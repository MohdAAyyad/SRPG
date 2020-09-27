// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCharacter.h"
#include "Grid/Tile.h"
#include "Grid/GridManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Grid/Tile.h"
#include "PathComponent.h"
#include "Components/WidgetComponent.h"
#include "Animation/GridCharacterAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "BattleManager.h"
#include "../ExternalFileReader/ExternalFileReader.h"
#include "BattleController.h"
#include "Definitions.h"
#include "Crowd/BattleCrowd.h"
#include "Particles/ParticleSystemComponent.h"
#include "CollisionQueryParams.h"
#include "StatsComponent.h"
#include "Intermediary/Intermediate.h"
#include "BattleController.h"
#include "TimerManager.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGridCharacter::AGridCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->AddRelativeLocation(FVector(-10.f,0.0f,20.0f));
	CameraBoom->RelativeRotation = FRotator(-25.0f, -60.0f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	TopDownCameraComponent->SetActive(false);


	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bMoving = false;

	pathComp = CreateDefaultSubobject<UPathComponent>(TEXT("Path Component"));

	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	widgetComp->SetupAttachment(RootComponent);
	widgetComp->SetVisibility(false);

	overheadWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Overhead Widget Component"));
	overheadWidgetComp->SetupAttachment(RootComponent);
	overheadWidgetComp->SetVisibility(false);
	overheadWidgetComp->AddRelativeLocation(FVector(0.0f, 0.0f, 100.0f));

	champParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Champion Particles"));
	champParticles->SetupAttachment(RootComponent);
	champParticles->AddRelativeLocation(FVector(0.0f, 0.0f, -90.0f));

	villainParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Villain Particles"));
	villainParticles->SetupAttachment(RootComponent);
	villainParticles->AddRelativeLocation(FVector(0.0f, 0.0f, 160.0f));
	villainParticles->AddRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));


	fileReader = CreateDefaultSubobject<UExternalFileReader>(TEXT("File Reader"));

	statsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));

	GetCapsuleComponent()->SetCollisionProfileName("GridCharacter");

	btlManager = nullptr;

	currentState = EGridCharState::IDLE;
	bChampion = bVillain = false;


	fighterIndex = 0;

	bHasMoved = false;
	bHasDoneAnAction = false;

	bpID = 0;

}

void AGridCharacter::BeginPlay()
{
	Super::BeginPlay();

	animInstance = Cast<UGridCharacterAnimInstance>(GetMesh()->GetAnimInstance());

	if(champParticles)
		champParticles->DeactivateSystem();

	if(villainParticles)
		villainParticles->DeactivateSystem();

	btlCtrl = Cast<ABattleController>(GetWorld()->GetFirstPlayerController());

}

void AGridCharacter::SetBtlAndCrdManagers(ABattleManager* btlManager_, ABattleCrowd* crd_)
{
	btlManager = btlManager_;
	crdManager = crd_;
	if (statsComp)
	{
		statsComp->AddTempCRD(CRD_DEP); //Deployment adds points
	}
	ATile* tile = GetMyTile();
	if(tile)
		tile->SetOccupied(true);
}

// Called every frame
void AGridCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bMoving && movementPath.Num() > 0)
	{
		MoveAccordingToPath();
	}

}

void AGridCharacter::Selected()
{
	if (originTile)
	{
		if(movementPath.Num()>0)
			movementPath.Empty();
		if(pathComp)
			originTile->GetGridManager()->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(),TILE_MOV,0);
	}
}
void AGridCharacter::NotSelected()
{
	if (originTile)
	{
		originTile->GetGridManager()->ClearHighlighted();
	}
}

void AGridCharacter::MoveAccordingToPath()
{
	if (movementPath.Num() > 0)
	{
		FVector direction = movementPath[movementPath.Num() - 1] - GetActorLocation();
		if (FMath::Abs(movementPath[movementPath.Num() - 1].X - GetActorLocation().X) > 10.0f || FMath::Abs(movementPath[movementPath.Num() - 1].Y - GetActorLocation().Y) > 10.0f)
		{
			AddMovementInput(direction, 1.0f);
		}
		else
		{
			movementPath.RemoveAt(movementPath.Num() - 1);
			if (movementPath.Num() == 0)
			{
				bMoving = false;
				ATile* tile_ = GetMyTile();
				if (tile_)
					tile_->SetOccupied(true);
				if (btlCtrl)
					btlCtrl->ResetViewLock();
				bHasMoved = true;
				if (bHasDoneAnAction) //Has moved and has done an action, we're done
				{
					FinishState();
				}
				else
				{
					NotSelected();
					Selected();
				}
			}
		}
	}
}

void AGridCharacter::UpdateOriginTile()
{
	originTile = GetMyTile();
	if(originTile)
		originTile->SetOccupied(true);
}

void AGridCharacter::MoveToThisTile(ATile* target_)
{
	if (target_)
	{
		if (pathComp)
		{
			pathComp->SetTargetTile(target_);
			pathComp->SetCurrentTile(GetMyTile());
			movementPath = pathComp->GetPath();
			bMoving = true;
		}
	}
}

void AGridCharacter::AttackUsingWeapon(AGridCharacter* target_, float delay_)
{
	if (target_)
	{
		actionTargets.Push(target_);
		FVector direction = target_->GetActorLocation() - GetActorLocation();
		FRotator rot = direction.Rotation();
		rot.Roll = GetActorRotation().Roll;
		rot.Pitch = GetActorRotation().Pitch;
		SetActorRotation(rot);
		FTimerHandle attackDelayHandle;
		//The delay here is needed to allow the camera enough time to focus on the character attacking
		GetWorld()->GetTimerManager().SetTimer(attackDelayHandle, this, &AGridCharacter::PlayAnimationAttackUsingWeapon, delay_, false);
		bHasDoneAnAction = true;
	}
}

void AGridCharacter::PlayAnimationAttackUsingWeapon()
{
	if (animInstance)
	{
		animInstance->WeaponAttack();
		//The animation event calls ActivateWeaponAttack
	}
}

void AGridCharacter::AttackUsingSkill(TArray<AGridCharacter*> targets_, float delay_)
{
	if (targets_.Num()>0)
	{
		if (targets_[0]) //Rotate towards one of the targets
		{
			actionTargets = targets_;
			FVector direction = actionTargets[0]->GetActorLocation() - GetActorLocation();
			FRotator rot = direction.Rotation();
			rot.Roll = GetActorRotation().Roll;
			rot.Pitch = GetActorRotation().Pitch;
			SetActorRotation(rot);

			FTimerHandle attackDelayHandle;
			//The delay here is needed to allow the camera enough time to focus on the character attacking
			GetWorld()->GetTimerManager().SetTimer(attackDelayHandle, this, &AGridCharacter::PlayAnimationAttackUsingSkill, delay_, false);
			bHasDoneAnAction = true;
		}
	}
}

void AGridCharacter::PlayAnimationAttackUsingSkill()
{
	if (animInstance) //Play the animation
	{
		animInstance->SkillAttack(chosenSkillAnimIndex);
		//The animation event calls ActivateSkillAttack
	}
}

ATile* AGridCharacter::GetMyTile()
{
	FHitResult hit;
	FVector end = GetActorLocation();
	end.Z -= 400.0f;
	FCollisionQueryParams queryParms(FName(TEXT("query")), false, this);
	queryParms.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), end, ECollisionChannel::ECC_Visibility, queryParms))
	{
		ATile* tile_ = Cast<ATile>(hit.Actor);
		if (tile_)
			return tile_;
	}

	return nullptr;
}

void AGridCharacter::SetMoving(bool value_)
{
	bMoving = value_;
}

AGridCharacter::EGridCharState AGridCharacter::GetCurrentState()
{
	return currentState;
}

void AGridCharacter::GridCharTakeDamage(float damage_, AGridCharacter* attacker_)
{

	//Rotate the character to face the attacker
	FVector direction = attacker_->GetActorLocation() - GetActorLocation();
	FRotator rot = direction.Rotation();
	rot.Roll = GetActorRotation().Roll;
	rot.Pitch = GetActorRotation().Pitch;
	SetActorRotation(rot);
	if (animInstance)
		animInstance->GotHit();

	//Health details are handled inside the children's code
}


void AGridCharacter::GridCharReactToSkill(float value_, int statIndex_, int statuEffectIndex_, AGridCharacter* attacker_)
{
	//PLACEHOLDER
	//Rotate the character to face the attacker
	FVector direction = attacker_->GetActorLocation() - GetActorLocation();
	FRotator rot = direction.Rotation();
	rot.Roll = GetActorRotation().Roll;
	rot.Pitch = GetActorRotation().Pitch;
	SetActorRotation(rot);
	if (animInstance)
		animInstance->GotHit();

	//TODO
	//Affect the correct stat 
	//Play the correct reaction animation
	//Apply the status effects should they exist
}




TArray<FItemTableStruct> AGridCharacter::GetOwnedItems()
{
	if (fileReader)
		return fileReader->GetAllOwnedItems();

	return TArray<FItemTableStruct>();
}

void AGridCharacter::HighlightItemUsage(FName itemName_)
{
	ATile* tile_ = GetMyTile();
	if (tile_)
	{
		tile_->GetGridManager()->ClearHighlighted();
		tile_->GetGridManager()->UpdateCurrentTile(tile_, 1, 2, TILE_ITM, 0); //Items always cover 1 tile only
		chosenItem = fileReader->ConvertItemNameToNameUsedInTable(itemName_);
		currentState = EGridCharState::HEALING;
	}
}

void  AGridCharacter::UseItemOnOtherChar(AGridCharacter* target_)
{
	if (target_ && fileReader)
	{
		target_->GridCharReactToItem(fileReader->GetItemStatIndex(3,chosenItem), fileReader->GetItemValue(chosenItem));
		fileReader->AddOwnedValueItemTable(chosenItem, 3, -1);
		bHasDoneAnAction = true;
		if (bHasMoved) //Has moved and has done an action, we're done
		{
			FinishState();
		}
		else
		{
			NotSelected();
			Selected();
		}
	}

}

void AGridCharacter::GridCharReactToItem(int statIndex_, int value_)
{
	if (animInstance)
		animInstance->SetUseItem();
}


float AGridCharacter::GetStat(int statIndex_)
{
	//Pass the statindex_ to the stats component
	return static_cast<float>(statsComp->GetStatValue(statIndex_));
}

void AGridCharacter::AddEquipmentStats(int tableIndex_)
{

	//Get the skills index for the equipped weapon and armor and pass them to the stats
	//Pass the weapon's HIT stat as the last value

	FEquipmentTableStruct weapon;
	FEquipmentTableStruct armor;
	FEquipmentTableStruct accessory;

	//Get the stats of the equipment and add them to the character's stats
	if (fileReader)
	{
		weapon = fileReader->GetEquipmentById(tableIndex_, statsComp->GetStatValue(STAT_WPN), EQU_WPN, statsComp->GetStatValue(STAT_WPI));
		armor = fileReader->GetEquipmentById(tableIndex_, statsComp->GetStatValue(STAT_ARM), EQU_ARM, statsComp->GetStatValue(STAT_ARI));
		accessory = fileReader->GetEquipmentById(tableIndex_, statsComp->GetStatValue(STAT_ACC), EQU_ACC, -1);
		
		statsComp->UpdateMaxHpAndMaxPip(weapon.hp + armor.hp + accessory.hp, weapon.pip + armor.pip + accessory.pip);
		statsComp->AddToStat(STAT_HP, weapon.hp + armor.hp + accessory.hp);
		statsComp->AddToStat(STAT_PIP, weapon.pip + armor.pip + accessory.pip);
		statsComp->AddToStat(STAT_ATK, weapon.atk + armor.atk + accessory.atk);
		statsComp->AddToStat(STAT_DEF, weapon.def + armor.def + accessory.def);
		statsComp->AddToStat(STAT_INT, weapon.intl + armor.intl + accessory.intl);
		statsComp->AddToStat(STAT_SPD, weapon.spd + armor.spd + accessory.spd);
		statsComp->AddToStat(STAT_CRT, weapon.crit + armor.crit + accessory.crit);
		statsComp->AddToStat(STAT_CRD, weapon.crd + armor.crd + accessory.crd);
		statsComp->AddToStat(STAT_WSI, weapon.skillsIndex);
		statsComp->AddToStat(STAT_WSN, weapon.skillsN);
		statsComp->AddToStat(STAT_ASI, armor.skillsIndex);
		statsComp->AddToStat(STAT_ASN, armor.skillsN);
		statsComp->AddToStat(STAT_WRS, weapon.range);
		statsComp->AddToStat(STAT_WDS, weapon.range + 1);
		statsComp->AddToStat(STAT_PURE, weapon.pure);
	}
		if (pathComp)
			pathComp->UpdateSpeed(statsComp->GetStatValue(STAT_SPD));
}
void  AGridCharacter::SetChampionOrVillain(bool value_) //True champion, false villain
{
	if (value_)
	{
		bChampion = true;
		villainParticles->DeactivateSystem();
		if (champParticles)
			champParticles->ActivateSystem(true);

	}
	else
	{
		bVillain = true;
		if (champParticles)
			champParticles->DeactivateSystem();
		if (villainParticles)
			villainParticles->ActivateSystem(true);

	}
}


void AGridCharacter::UnElect()
{
	bChampion = bVillain = false;
	if (champParticles)
		champParticles->DeactivateSystem();
	if (villainParticles)
		villainParticles->DeactivateSystem();
}

void AGridCharacter::ResetCameraFocus()
{
	//Called from the UI once an attack finishes
	if (btlCtrl)
		btlCtrl->ResetFocus();
}


void AGridCharacter::Die()
{
	Destroy();
}

void AGridCharacter::SetFighterName(FString name_)
{
	pName = name_;
}

UExternalFileReader*  AGridCharacter::GetFileReader()
{
	return fileReader;
}

UStatsComponent* AGridCharacter::GetStatsComp()
{
	return statsComp;
}

AGridManager* AGridCharacter::GetGridManager()
{
	return originTile->GetGridManager();
}

UPathComponent* AGridCharacter::GetPathComponent()
{
	return pathComp;
}

ATile* AGridCharacter::GetOriginTile()
{
	return originTile;
}