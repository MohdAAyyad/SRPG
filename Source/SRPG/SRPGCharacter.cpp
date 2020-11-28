// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SRPGCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "SRPGPlayerController.h"
#include "Engine/World.h"
#include "Networking/SRPGGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"


ASRPGCharacter::ASRPGCharacter()
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

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// create the widget component
	widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SetReplicates(true);
	
}

void ASRPGCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CursorToWorld != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, false, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);			
		}
	}
}

void ASRPGCharacter::SetInteracting(IInteractable* ref_)
{
	interacting = ref_;
}

void ASRPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	//ASRPGPlayerController* controller = Cast<ASRPGPlayerController>(GetController());
	SetupController();
}

void ASRPGCharacter::SetupController_Implementation()
{
	ASRPGPlayerController* controller = Cast<ASRPGPlayerController>(GetController());

	//if (HasAuthority())
	//{
	//	controller = Cast<ASRPGPlayerController>(GetWorld()->GetFirstPlayerController());
	//}
	//else
	//{
	//	ASRPGGameState* state = Cast<ASRPGGameState>(GetWorld()->GetGameState());
	//	controller = Cast<ASRPGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), state->GetCurrentControllerIndex()));
	//	state->SetCurrentControllerIndex(state->GetCurrentControllerIndex() + 1);
	//}

	//UE_LOG(LogTemp, Warning, TEXT("controllers %d"), GetWorld()->GetNumPlayerControllers());
	//if (controller)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Player ref set"));

	//}

	if (controller)
	{
		controller->SetPlayerReference(this);
	}

}

bool ASRPGCharacter::SetupController_Validate()
{
	return true;
}

void ASRPGCharacter::TriggerPauseMenu()
{
	if (widget && widget->GetUserWidgetObject()->IsInViewport() == false)
	{
		widget->GetUserWidgetObject()->AddToViewport();
	}
	else if (widget && widget->GetUserWidgetObject()->IsInViewport() == true)
	{
		widget->GetUserWidgetObject()->RemoveFromViewport();
	}
}

TArray<FEquipmentTableStruct> ASRPGCharacter::GetAllOwnedEquipment(FString tableName_)
{
	FName convert = FName(*tableName_);
	return fileReader->FindAllOwnedEquipment(fileReader->FindTableIndexByName(convert));
}

TArray<FFighterTableStruct> ASRPGCharacter::GetAllFighters(FString tableName_)
{
	FName convert = FName(*tableName_);
	return fileReader->GetAllRecruitedFighters(fileReader->FindTableIndexByName(convert));
}

TArray<UTexture*> ASRPGCharacter::GetAllItemTextures()
{
	return itemTextures;
}

TArray<UTexture*> ASRPGCharacter::GetAllFighterTextures()
{
	return fighterTextures;
}

bool ASRPGCharacter::CanFighterEquipWeapon(FString tableName_, int id_)
{
	FName convert = FName(*FString::FromInt(id_));
	FFighterTableStruct row = fileReader->FindFighterRowById(fileReader->FindTableIndexByName(convert), id_);

	if (row.equippedWeapon == 0)
	{
		// fighter already has something equipped in this slot
		return true;
	}
	else
	{
		return false;
	}
}

bool ASRPGCharacter::CanFighterEquipArmor(FString tableName_, int id_)
{
	FName convert = FName(*FString::FromInt(id_));
	FFighterTableStruct row = fileReader->FindFighterRowById(fileReader->FindTableIndexByName(convert), id_);

	if (row.equippedArmor == 0)
	{
		// fighter already has something equipped in this slot
		return true;
	}
	else
	{
		return false;
	}
}

bool ASRPGCharacter::CanFighterEquipAccessory(FString tableName_, int id_)
{
	FName convert = FName(*FString::FromInt(id_));
	FFighterTableStruct row = fileReader->FindFighterRowById(fileReader->FindTableIndexByName(convert), id_);

	if (row.equippedAccessory == 0)
	{
		// fighter already has something equipped in this slot
		return true;
	}
	else
	{
		return false;
	}
}



