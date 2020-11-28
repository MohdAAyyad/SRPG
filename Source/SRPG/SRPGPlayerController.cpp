// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SRPGPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "SRPGCharacter.h"
#include "Hub/NPC.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"


ASRPGPlayerController::ASRPGPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ASRPGPlayerController::SetPlayerReference_Implementation(ASRPGCharacter* ref_)
{
	player = ref_;
}

bool ASRPGPlayerController::SetPlayerReference_Validate(ASRPGCharacter* ref_)
{
	return true;
}

void ASRPGPlayerController::CheckCollisionUnderMouse()
{
	FHitResult hit;
	GetHitResultUnderCursor(ECC_Visibility, false, hit);
	if (hit.GetActor() != nullptr)
	{
		IInteractable* hitInteractable = Cast<IInteractable>(hit.GetActor());
		if (hitInteractable)
		{
			// remove the previous interacting 
			if (interacting)
			{
				interacting->UnInteract();
				interacting = nullptr;
				player->SetInteracting(nullptr);
			}
			//set a new one
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

void ASRPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	shouldMove = false;
	radius = 100.0f;
}

void ASRPGPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor || shouldMove)
	{
		SetNewMoveDestination(target.ImpactPoint);
	}
}

void ASRPGPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ASRPGPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ASRPGPlayerController::OnSetDestinationReleased);
	InputComponent->BindAction("LeftMouse", IE_Pressed, this, &ASRPGPlayerController::CheckCollisionUnderMouse);
	InputComponent->BindAction("SpaceBar", IE_Pressed, this, &ASRPGPlayerController::AddPauseMenuToViewport);
}

void ASRPGPlayerController::AddPauseMenuToViewport()
{
	if (player)
	{
		player->TriggerPauseMenu();
	}
}


void ASRPGPlayerController::MoveToMouseCursor_Implementation()
{	
	// Trace to see what is under the mouse cursor
	GetHitResultUnderCursor(ECC_Visibility, false, target);

	if (shouldMove)
	{
		//shouldMove = true;
		
	}
}

bool ASRPGPlayerController::MoveToMouseCursor_Validate()
{
	return true;
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


void ASRPGPlayerController::SetNewMoveDestination_Implementation(const FVector DestLocation)
{
	
	if (shouldMove)
	{
		APawn* const MyPawn = GetPawn();
		if (MyPawn)
		{
			//UNavigationSystemV1* const nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
			float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

			// We need to issue move command only if far enough in order for walk animation to play correctly

			if (player == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("player Ref is nullptr"));
			}

			//float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());
			if ((Distance > radius))
			{
				FVector result = (DestLocation - MyPawn->GetActorLocation());
				result.Normalize();
				result *= MyPawn->GetMovementComponent()->GetMaxSpeed();
				// this function doesn't like MP so we'll have to create a new function for moving characters
				//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
				MyPawn->AddMovementInput(result);

				//nav->SimpleMoveToLocation(this, DestLocation);
				//MyPawn->AddMovementInput(DestLocation);
			}

			if(Distance < radius)
			{
				shouldMove = false;
			}
		}
	}
	
	
}

bool ASRPGPlayerController::SetNewMoveDestination_Validate(const FVector DestLocation)
{
	return true;
}

void ASRPGPlayerController::OnSetDestinationPressed_Implementation()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
	shouldMove = true;
	MoveToMouseCursor();
}

bool ASRPGPlayerController::OnSetDestinationPressed_Validate()
{
	return true;
}

void ASRPGPlayerController::OnSetDestinationReleased_Implementation()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

bool ASRPGPlayerController::OnSetDestinationReleased_Validate()
{
	return true;
}

