// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SRPGPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "SRPGCharacter.h"
#include "Hub/NPC.h"
#include "Engine/World.h"

ASRPGPlayerController::ASRPGPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ASRPGPlayerController::SetPlayerReference(ASRPGCharacter* ref_)
{
	player = ref_;
}

void ASRPGPlayerController::CheckCollisionUnderMouse()
{
	//UE_LOG(LogTemp, Warning, TEXT("Check Collision has been called"));
	FHitResult hit;
	GetHitResultUnderCursor(ECC_Visibility, false, hit);
	if (hit.GetActor() != nullptr)
	{
		IInteractable* hitInteractable = Cast<IInteractable>(hit.GetActor());
		if (hitInteractable)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Interact has been cast!"));
			interacting = hitInteractable;
			hitInteractable->Interact();
			player->SetInteracting(interacting);
		}
		else
		{
			if (interacting != nullptr)
			{
				//UE_LOG(LogTemp, Error, TEXT("uninteracting"));
				interacting->UnInteract();
				interacting = nullptr;
				player->SetInteracting(nullptr);
			}
		}
	}
}

void ASRPGPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void ASRPGPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ASRPGPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ASRPGPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASRPGPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ASRPGPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("LeftMouse", IE_Pressed, this, &ASRPGPlayerController::CheckCollisionUnderMouse);
}

void ASRPGPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASRPGPlayerController::MoveToMouseCursor()
{	
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void ASRPGPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ASRPGPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance >50.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ASRPGPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ASRPGPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

