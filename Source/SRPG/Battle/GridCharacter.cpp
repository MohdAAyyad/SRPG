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
#include "ExternalFileReader/FighterTableStruct.h"
#include "Weapons/WeaponBase.h"
#include "Grid/Obstacle.h"
#include "ProjectileGridCharacter.h"
#include "Components/PostProcessComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

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
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

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
	TopDownCameraComponent->AddRelativeLocation(FVector(-360.0f, 0.0f, 0.0f));
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
	overheadWidgetComp->AddRelativeLocation(FVector(0.0f, 0.0f, 100.0f));

	champParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Champion Particles"));
	champParticles->SetupAttachment(RootComponent);
	champParticles->AddRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	champParticles->SetAutoActivate(false);

	villainParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Villain Particles"));
	villainParticles->SetupAttachment(RootComponent);
	villainParticles->AddRelativeLocation(FVector(0.0f, 0.0f, 160.0f));
	villainParticles->AddRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	villainParticles->SetAutoActivate(false);

	permaChampParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Perma Champion Particles"));
	permaChampParticles->SetupAttachment(RootComponent);
	permaChampParticles->AddRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	permaChampParticles->SetAutoActivate(false);


	fileReader = CreateDefaultSubobject<UExternalFileReader>(TEXT("File Reader"));

	audioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	audioComponent->SetupAttachment(RootComponent);

	statsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));

	GetCapsuleComponent()->SetCollisionProfileName("GridCharacter");

	btlManager = nullptr;

	currentState = EGridCharState::IDLE;
	championIndex = -1;
	championBuffCount = 0;


	fighterIndex = 0;
	fighterID = 0;
	bHasMoved = false;
	bHasDoneAnAction = false;

	bpID = 0;

	bReplicates = true;
	bReplicateMovement = true;

	//Post process
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(0);

	ppComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	ppComponent->SetupAttachment(RootComponent);
}

void AGridCharacter::BeginPlay()
{
	Super::BeginPlay();

	animInstance = Cast<UGridCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	btlCtrl = Cast<ABattleController>(GetWorld()->GetFirstPlayerController());

	FTimerHandle postInitHandle;
	GetWorld()->GetTimerManager().SetTimer(postInitHandle, this, &AGridCharacter::PostInit, 0.3f, false);

}

void AGridCharacter::PostInit()
{
	if (champParticles)
		champParticles->DeactivateSystem();

	if (villainParticles)
		villainParticles->DeactivateSystem();

	if (permaChampParticles)
		permaChampParticles->DeactivateSystem();

	//Spawn and attack the weapons
	FVector loc_ = GetActorLocation();
	loc_.X += 200.0f;
	if (weaponMeshes.Num() > 0)
	{
		if (weaponMeshes[0])
		{
			FName weaponSocketName = TEXT("WeaponSocket");
			AWeaponBase* weapon = GetWorld()->SpawnActor<AWeaponBase>(weaponMeshes[0], loc_,FRotator::ZeroRotator);
			weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, weaponSocketName);
			equippedWeapons.Push(weapon);
		}
	}

	if (weaponMeshes.Num() > 1)
	{
		if (weaponMeshes[1])
		{
			FName shieldSocketName = TEXT("ShieldSocket");
			AWeaponBase* weapon = GetWorld()->SpawnActor<AWeaponBase>(weaponMeshes[1], loc_, FRotator::ZeroRotator);
			weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, shieldSocketName);
			equippedWeapons.Push(weapon);
		}
	}

	UpdateOriginTile();
}

void AGridCharacter::SetBtlAndCrdManagers(ABattleManager* btlManager_, ABattleCrowd* crd_)
{
	btlManager = btlManager_;
	crdManager = crd_;
	if (statsComp)
	{
		if (statsComp->AddTempCRD(CRD_DEP))//Deployment adds points
		{
			crdManager->UpdateFavor(true);
		}
	}
	ATile* tile = GetMyTile();
	if(tile)
		tile->SetOccupied(true);

	myHoverInfo.targetIndex = HOVER_PLY;
	myHoverInfo.hpPercentage = 1.0f;
	myHoverInfo.pipPercentage = 1.0f;
	myHoverInfo.textureID = bpID;
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
			movementPath = pathComp->GetPathToTargetTile(TILE_MOV);
			if(movementPath.Num()>0)
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

void AGridCharacter::AttackUsingWeapon(AObstacle* obstacle_, float delay_)
{
	if (obstacle_)
	{
		targetObstacle = obstacle_;
		FVector direction = obstacle_->GetActorLocation() - GetActorLocation();
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

void AGridCharacter::AttackUsingSkill(TArray<AGridCharacter*> targets_, float delay_, AObstacle* obstacle_)
{
	targetObstacle = obstacle_;
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
	else if (targetObstacle)
	{
		FVector direction = targetObstacle->GetActorLocation() - GetActorLocation();
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

void AGridCharacter::AttackUsingSkill(float delay_)
{
	if (actionTargets.Num() > 0)
	{
		if (actionTargets[0]) //Rotate towards one of the targets
		{
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
		{
			return tile_;
		}
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

void AGridCharacter::GridCharTakeDamage(float damage_, AGridCharacter* attacker_, bool crit_, int statusEffect_)
{
	//Rotate the character to face the attacker
	FVector direction = attacker_->GetActorLocation() - GetActorLocation();
	FRotator rot = direction.Rotation();
	rot.Roll = GetActorRotation().Roll;
	rot.Pitch = GetActorRotation().Pitch;
	SetActorRotation(rot);

	//Health details are handled inside the children's code
}


void AGridCharacter::GridCharReactToSkill(float damage_, int statIndex_, int statuEffectIndex_, AGridCharacter* attacker_, bool crit_)
{
	//Rotate the character to face the attacker
	FVector direction = attacker_->GetActorLocation() - GetActorLocation();
	FRotator rot = direction.Rotation();
	rot.Roll = GetActorRotation().Roll;
	rot.Pitch = GetActorRotation().Pitch;
	SetActorRotation(rot);


	//Stats are affected in children
}

void AGridCharacter::GridCharReatToElemental(float damage_, int statusEffectIndex_)
{
	//Overriden in children

}




TArray<FItemTableStruct> AGridCharacter::GetOwnedItems()
{
	if (fileReader)
		return fileReader->GetAllOwnedItems(5);

	return TArray<FItemTableStruct>();
}

//Called from UI
void AGridCharacter::HighlightItemUsage(FItemTableStruct item_)
{
	ATile* tile_ = GetMyTile();
	if (tile_)
	{
		tile_->GetGridManager()->ClearHighlighted();
		tile_->GetGridManager()->UpdateCurrentTile(tile_, 1, 2, TILE_ITM, 0); //Items always cover 1 tile only
		chosenItem = item_;
		currentState = EGridCharState::HEALING;
		btlCtrl->SetHoverTargeting(true);
	}
}

void  AGridCharacter::UseItemOnOtherChar(AGridCharacter* target_)
{
	if (target_ && fileReader)
	{
		target_->GridCharReactToItem(chosenItem.statIndex, chosenItem.value);
		fileReader->AddOwnedValueItemTable(5, chosenItem.id, -1);
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
	if (audioComponent)
	{
		if (!audioComponent->IsPlaying())
		{
			if (soundEffects.Num() >= 6)
			{
				audioComponent->Sound = soundEffects[5];
				audioComponent->Play();
			}
		}
	}
	if (animInstance)
		animInstance->SetUseItem();

	if (statsComp)
		statsComp->AddToStat(statIndex_, value_);
}


float AGridCharacter::GetStat(int statIndex_)
{
	//Pass the statindex_ to the stats component
	return static_cast<float>(statsComp->GetStatValue(statIndex_));
}

void AGridCharacter::PassInYourFinalStatsToTheIntermediate(TArray<int>& stats_)
{
	FFighterTableStruct fighter_;
	fighter_.hp = statsComp->GetMaxHP();
	fighter_.pip = statsComp->GetMaxPIP();
	fighter_.atk = stats_[STAT_ATK];
	fighter_.def = stats_[STAT_DEF];
	fighter_.intl = stats_[STAT_INT];
	fighter_.spd = stats_[STAT_SPD];
	fighter_.agl = stats_[STAT_HIT];
	fighter_.crit = stats_[STAT_CRT];
	fighter_.crd = stats_[STAT_CRD];
	fighter_.currentEXP = stats_[STAT_EXP];
	fighter_.neededEXPToLevelUp = stats_[STAT_NXP];
	fighter_.level = stats_[STAT_LVL];
	fighter_.id = fighterID;

	Intermediate::GetInstance()->AddFighterToSelected(fighter_); //Selected fighters is used again. This time to update the recruited table. The fighter shop will take care of that.
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
		weapon = fileReader->GetEquipmentById(2, statsComp->GetStatValue(STAT_WPN), EQU_WPN, statsComp->GetStatValue(STAT_WPI));
		armor = fileReader->GetEquipmentById(3, statsComp->GetStatValue(STAT_ARM), EQU_ARM, statsComp->GetStatValue(STAT_ARI));
		accessory = fileReader->GetEquipmentById(4, statsComp->GetStatValue(STAT_ACC), EQU_ACC, -1);
		
		statsComp->UpdateMaxHpAndMaxPip(weapon.hp + armor.hp + accessory.hp, weapon.pip + armor.pip + accessory.pip);
		statsComp->AddToStat(STAT_HP, weapon.hp + armor.hp + accessory.hp);
		statsComp->AddToStat(STAT_PIP, weapon.pip + armor.pip + accessory.pip);
		statsComp->AddToStat(STAT_ATK, weapon.atk + armor.atk + accessory.atk);
		statsComp->AddToStat(STAT_DEF, weapon.def + armor.def + accessory.def);
		statsComp->AddToStat(STAT_INT, weapon.intl + armor.intl + accessory.intl);
		statsComp->AddToStat(STAT_SPD, weapon.spd + armor.spd + accessory.spd);
		statsComp->AddToStat(STAT_CRT, weapon.crit + armor.crit + accessory.crit);
		statsComp->AddToStat(STAT_HIT, armor.hit + accessory.hit); //Agility is not affected by the weapon
		statsComp->AddToStat(STAT_WHT, weapon.hit); //Weapon hit is affected by the weapon's hit
		statsComp->AddToStat(STAT_CRD, weapon.crd + armor.crd + accessory.crd);
		statsComp->AddToStat(STAT_WSI, weapon.skillsIndex);
		statsComp->AddToStat(STAT_WSN, weapon.skillsN);
		statsComp->AddToStat(STAT_ASI, armor.skillsIndex);
		statsComp->AddToStat(STAT_ASN, armor.skillsN);
		statsComp->AddToStat(STAT_WRS, weapon.range);
		statsComp->AddToStat(STAT_WDS, weapon.range + 1);
		statsComp->AddToStat(STAT_PURE, weapon.pure);
		statsComp->AddToStat(STAT_WPN_EFFECT, weapon.statusEffect);
		statsComp->AddToStat(STAT_ARM_EFFECT, armor.statusEffect);
		statsComp->AddToStat(STAT_ACC_EFFECT, accessory.statusEffect);
	}
		if (pathComp)
			pathComp->UpdateSpeed(statsComp->GetStatValue(STAT_SPD));
}
void  AGridCharacter::SetChampionOrVillain(bool value_) //True champion, false villain
{
	if (value_)
	{
		championIndex = 0;
		if (villainParticles)
			villainParticles->DeactivateSystem();
		if (champParticles)
			champParticles->ActivateSystem(true);
		statsComp->UpdateChampionVillainStats(true);
		championBuffCount = 1;

	}
	else
	{
		championIndex = 1;
		if (champParticles)
			champParticles->DeactivateSystem();
		if (villainParticles)
			villainParticles->ActivateSystem(true);
		statsComp->UpdateChampionVillainStats(false);
		championBuffCount = 1;

	}
}


void AGridCharacter::UnElect()
{
	if (championIndex == 0)
	{
		statsComp->RevertChampionVillainStatsUpdate(true);
		if (champParticles)
			champParticles->DeactivateSystem();
	}
	else if (championIndex == 1)
	{
		statsComp->RevertChampionVillainStatsUpdate(false);
		if (villainParticles)
			villainParticles->DeactivateSystem();
	}
	championIndex = -1;
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
void AGridCharacter::YouHaveKilledYouTarget(bool killedAnEnemy_)
{
	//killedAnEnemy_ represents the dead character. So we update the favor in the opposite direction
	if (statsComp->AddTempCRD(CRD_KIL))
	{
		crdManager->UpdateFavor(!killedAnEnemy_);
	}

	statsComp->AddToStat(STAT_EXP, KILL_RWRD);

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

void AGridCharacter::RemoveOverheadWidget()
{
	overheadWidgetComp->SetVisibility(false);
}

void AGridCharacter::GridCharReactToMiss()
{
	animInstance->GotMiss();
	overheadWidgetComp->SetVisibility(true);
}

void AGridCharacter:: YouHaveJustKilledAChampion(int championIndex_)
{
	if (championIndex_ == 0 || championIndex_ == 2) //Killed a champion
	{
		if (championIndex == 1) //If I am a villain
		{
			//If the villain defeats the champion, the hype meter ratio flips (30:70)
			crdManager->FlipFavorMeter();
			//the villain becomes the champion until the end of the match (permanent), 
			championIndex = 2;
			crdManager->ChampVillainIsDead(false); //No longer a villain
			crdManager->IAmTheNewChampion(this); //I am the champion now
			crdManager->SetPermaChampion(true); //and I'm inevitable...I mean permanent

			//Update the stats
			statsComp->RevertChampionVillainStatsUpdate(false); //Rever the villain nerf
			statsComp->AddToStat(STAT_PIP, -VILL_PIP); //The revert function does not revert the pips
			statsComp->UpdateChampionVillainStats(true); //Triple champ buff
			statsComp->UpdateChampionVillainStats(true);
			statsComp->UpdateChampionVillainStats(true);
			championBuffCount = 3;
			//Its CRD stat also increases by 3.
			statsComp->AddToStat(STAT_CRD, 3);
			crdManager->UpdateFavorForChamp(this,3);

			//Update the played particles
			if (champParticles)
				champParticles->ActivateSystem(true);
			if (villainParticles)
				villainParticles->DeactivateSystem();
			if (permaChampParticles)
				permaChampParticles->ActivateSystem(true);

		}
		else if (championIndex == -1) //I'm neither a champion nor a villain
		{
			//If the champion (perma or otherwise) is defeated by a non-villain character and 
			//the ratio is 70:30 or higher in favor of the champion�s team, the hype meter flips. 
			crdManager->FlipFavorMeter(this);
			//The character that defeated it becomes the permanent champion and 
			crdManager->ChampVillainIsDead(true); //No longer a champion
			crdManager->ChampVillainIsDead(false); //No longer a villain)
			crdManager->IAmTheNewChampion(this);

			championIndex = 2;
			//and gets a boost to ATK, INT, SPD and a nerf to DEF. 
			statsComp->UpdateChampionVillainStats(true);
			statsComp->UpdateChampionVillainStats(true);
			//Its CRD stat also increases by 2. 
			statsComp->AddToStat(STAT_CRD, 2);
			crdManager->UpdateFavorForChamp(this, 2);

			championBuffCount = 2;

			//If there�s a current villain, //they lose their status and return everything to normal.
			crdManager->UnElect(false);

			//Update the played particles
			if (champParticles)
				champParticles->ActivateSystem(true);
			if (villainParticles)
				villainParticles->DeactivateSystem();
			if (permaChampParticles)
				permaChampParticles->ActivateSystem(true);


		}
	}
	else if (championIndex_ == 1) //Killed a villain
	{
		if (championIndex == 0) //If I am already a champion
		{
			//If the champion defeats the villain, the champion remains a champion until the end of the match (permanent)
			championIndex = 2;
			//and gets an extra boost to ATK and INT and an extra nerf to DEF. 
			statsComp->UpdateChampionVillainStats(true);
			//The champion�s CRD stat also automatically increases by 2.
			statsComp->AddToStat(STAT_CRD, 2);
			championBuffCount = 2;
			//Add favor twice as we got 2 crd points
			crdManager->UpdateFavorForChamp(this,2);
			crdManager->SetPermaChampion(true);

			//Update the played particles
			if (champParticles)
				champParticles->ActivateSystem(true);
			if (villainParticles)
				villainParticles->DeactivateSystem();
			if (permaChampParticles)
				permaChampParticles->ActivateSystem(true);
		}
		else if(championIndex == -1 && !crdManager->GetPermaStatus()) //I'm neither a champion, nor a villain
		{
			//If the villain is defeated by a non-champion character, 
			//the character that defeated it becomes the permanent champion and the original�s champion stats return to normal.
			crdManager->IAmTheNewChampion(this);
			crdManager->SetPermaChampion(true);
			championIndex = 2;
			//and gets a boost to ATK, INT, SPD and a nerf to DEF. 
			statsComp->UpdateChampionVillainStats(true);
			statsComp->UpdateChampionVillainStats(true);
			//Its CRD stat also increases by 2. 
			statsComp->AddToStat(STAT_CRD, 2);
			crdManager->UpdateFavorForChamp(this,2);
			championBuffCount = 2;
			//Update the played particles
			if (champParticles)
				champParticles->ActivateSystem(true);
			if (villainParticles)
				villainParticles->DeactivateSystem();
			if (permaChampParticles)
				permaChampParticles->ActivateSystem(true);
		}
	}
}

void AGridCharacter::YouAreTargetedByMeNow(AGridCharacter* ref_)
{
	if (!TargetedByTheseCharacters.Contains(ref_))
	{
		TargetedByTheseCharacters.Push(ref_);
	}
}
void AGridCharacter::YouAreNoLongerTargetedByMe(AGridCharacter* ref_)
{
	if (TargetedByTheseCharacters.Num() > 0)
	{
		if (TargetedByTheseCharacters.Contains(ref_))
		{
			TargetedByTheseCharacters.Remove(ref_);
		}
	}
}

void AGridCharacter::IamDeadStopTargetingMe()
{
	//Overriden in children
}

void AGridCharacter::ResetActionTargets()
{
	if(actionTargets.Num() > 0)
		actionTargets.Empty();
}

void AGridCharacter::SpawnProjectile(int index_, FVector spawnLoc_)
{
	if (index_ >= 0 && index_ < projectiles.Num())
	{
		if (projectiles[index_])
		{ 
			GetWorld()->SpawnActor<AProjectileGridCharacter>(projectiles[index_], spawnLoc_, GetActorRotation());
		}
	}
}

void AGridCharacter::ActivateOutline(bool value_)
{
	if(value_)
		GetMesh()->SetCustomDepthStencilValue(1);
	else
		GetMesh()->SetCustomDepthStencilValue(0);
}

void  AGridCharacter::TargetedOutline()
{
	GetMesh()->SetCustomDepthStencilValue(2);
}

void AGridCharacter::PlaySoundEffect(int index_)
{
	if (index_ >= 0 && index_ < soundEffects.Num())
	{
		if (!audioComponent->IsPlaying())
		{
			audioComponent->Sound = soundEffects[index_];
			audioComponent->Play();
		}
	}
}

//Hover info is used to update the hover UI
FHoverInfo AGridCharacter::GetMyHoverInfo()
{
	myHoverInfo.hpPercentage = statsComp->GetHPPercentage();
	myHoverInfo.pipPercentage = statsComp->GetPIPPercentage();
	return myHoverInfo;
}

void AGridCharacter::SetTargetHoverInfo(FHoverInfo info_)
{
	targetHoverInfo = info_;
}