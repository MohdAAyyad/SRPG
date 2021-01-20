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

void ASRPGPlayerController::SetPlayerReference(ASRPGCharacter* ref_)
{
	player = ref_;

	//Bind the input to the pause menu
	if (player)
		InputComponent->BindAction("Pause", IE_Pressed, player, &ASRPGCharacter::TriggerPauseMenu);

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
			if(player)
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
}


void ASRPGPlayerController::MoveToMouseCursor()
{	
	// Trace to see what is under the mouse cursor
	GetHitResultUnderCursor(ECC_Visibility, false, target);
}


void ASRPGPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	
	if (shouldMove)
	{
		if (player)
		{
			float const Distance = FVector::Dist(DestLocation, player->GetActorLocation());

			// We need to issue move command only if far enough in order for walk animation to play correctly

			if ((Distance > radius))
			{
				FVector result = (DestLocation - player->GetActorLocation());
				result.Normalize();
				result *= player->GetMovementComponent()->GetMaxSpeed();
				player->AddMovementInput(result);
			}

			if(Distance < radius)
			{
				shouldMove = false;
			}
		}
	}
	
	
}

void ASRPGPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
	shouldMove = true;
	MoveToMouseCursor();
}


void ASRPGPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void ASRPGPlayerController::FocusOnThisNPC(AActor* npc_, float rate_)
{
	if (npc_)
	{
		SetViewTargetWithBlend(npc_, rate_);
	}
}

ASRPGCharacter* ASRPGPlayerController::GetPlayerReference()
{
	if (player)
	{
		return player;
	}
	else
	{
		return nullptr;
	}
}

