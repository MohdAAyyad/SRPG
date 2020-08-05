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
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
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
	widgetComp = CreateAbstractDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	widgetComp->SetupAttachment(RootComponent);
	widgetComp->SetVisibility(false);

	fileReader = CreateDefaultSubobject<UExternalFileReader>(TEXT("File Reader"));

	GetCapsuleComponent()->SetCollisionProfileName("GridCharacter");

	btlManager = nullptr;

	currentState = EGridCharState::IDLE;
	chosenSkill = 0;
	chosenSkillAnimIndex = 0;

}

void AGridCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateOriginTile(); 
	animInstance = Cast<UGridCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	
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
			originTile->GetGridManager()->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(),0);
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
				ATile* tile_ = GetMyTile();
				if (tile_)
					tile_->SetOccupied(true);
				bMoving = false;
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
			if (originTile)
				originTile->SetOccupied(false);
			pathComp->SetTargetTile(target_);
			pathComp->SetCurrentTile(GetMyTile());
			movementPath = pathComp->GetPath();
			bMoving = true;
		}
	}
}

void AGridCharacter::AttackUsingWeapon(AGridCharacter* target_)
{
	if (target_)
	{
		actionTargets.Push(target_);
		FVector direction = target_->GetActorLocation() - GetActorLocation();
		FRotator rot = direction.Rotation();
		rot.Roll = GetActorRotation().Roll;
		rot.Pitch = GetActorRotation().Pitch;
		SetActorRotation(rot);
		if (animInstance)
		{
			animInstance->WeaponAttack();
		}
	}
}

void AGridCharacter::AttackUsingSkill(TArray<AGridCharacter*> targets_)
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

			if (animInstance) //Play the animation
			{
				animInstance->SkillAttack(chosenSkillAnimIndex);
			}
		}
	}
}

void AGridCharacter::SetBattleManager(ABattleManager* btlManager_)
{
	btlManager = btlManager_;
}

ATile* AGridCharacter::GetMyTile()
{
	FHitResult hit;
	FVector end = GetActorLocation();
	end.Z -= 400.0f;
	if (GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), end, ECollisionChannel::ECC_Visibility))
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
}

TArray<FSkillTableStruct>AGridCharacter::GetCharacterSkills()
{
	if (skills.Num() == 0) //No need to access the table every time we need to use a skill
	{
		if (fileReader)
		{
			TArray<FSkillTableStruct*> skillPtrs = fileReader->GetSkills(weaponIndex, currentLevel);
			for (auto n : skillPtrs)
			{
				skills.Push(*n);
			}
		}
	}

	return skills;
}


void AGridCharacter::UseSkill(int index_)
{
	//TODO check if we have enough pips first.
	if (index_ >= 0 && index_ < skills.Num())
	{
		ATile* tile_ = GetMyTile();
		if (tile_)
		{
			int rowSpeed_ = skills[index_].rge;
			int depth_ = rowSpeed_ + 1;
			chosenSkill = index_;
			chosenSkillAnimIndex = skills[index_].animationIndex;
			tile_->GetGridManager()->ClearHighlighted();
			tile_->GetGridManager()->UpdateCurrentTile(tile_, rowSpeed_, depth_, 6);
			currentState = EGridCharState::SKILLING;
			ABattleController* btlctrl = Cast< ABattleController>(GetWorld()->GetFirstPlayerController());
			if (btlctrl)
				btlctrl->SetTargetingWithSkill(true, skills[index_].rows, skills[index_].depths);
		}
	}
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
		tile_->GetGridManager()->UpdateCurrentTile(tile_, 1, 2, 2); //Items always cover 1 tile only
		chosenItem = fileReader->ConvertItemNameToNameUsedInTable(itemName_);
		currentState = EGridCharState::HEALING;
		UE_LOG(LogTemp, Warning, TEXT("Current state is healing"));
	}
}

void  AGridCharacter::UseItemOnOtherChar(AGridCharacter* target_)
{
	if (target_ && fileReader)
	{
		target_->GridCharReactToItem(fileReader->GetItemStatIndex(chosenItem), fileReader->GetItemValue(chosenItem));
		fileReader->AddOwnedValueItemTable(chosenItem, 1, -1);
	}

}

void AGridCharacter::GridCharReactToItem(int statIndex_, int value_)
{
	UE_LOG(LogTemp, Warning, TEXT("Reacting to item"));
	if (animInstance)
		animInstance->SetUseItem();
}
