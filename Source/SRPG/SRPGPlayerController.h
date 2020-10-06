// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Hub/Interactable.h"
#include "SRPGPlayerController.generated.h"

UCLASS()
class ASRPGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASRPGPlayerController();

	void SetPlayerReference(class ASRPGCharacter* ref_);

	void CheckCollisionUnderMouse();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	UFUNCTION(Unreliable, Server, WithValidation)
	void SetNewMoveDestination(const FVector DestLocation);
	void SetNewMoveDestination_Implementation(const FVector DestLocation);
	bool SetNewMoveDestination_Validate(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
	class ASRPGCharacter* player;
	IInteractable* interacting;
};


