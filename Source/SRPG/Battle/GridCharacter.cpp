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

	GetCapsuleComponent()->SetCollisionProfileName("GridCharacter");

	btlManager = nullptr;

	currentState = EGridCharState::IDLE;

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

void AGridCharacter::AttackThisTarget(AGridCharacter* target_)
{
	if (target_)
	{
		actionTarget = target_;
		FVector direction = target_->GetActorLocation() - GetActorLocation();
		FRotator rot = direction.Rotation();
		rot.Roll = GetActorRotation().Roll;
		rot.Pitch = GetActorRotation().Pitch;
		SetActorRotation(rot);
		if (animInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("Called anim instance weapon attack"));
			animInstance->WeaponAttack();
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
