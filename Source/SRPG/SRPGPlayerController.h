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

	UFUNCTION(Reliable, Server, WithValidation)
	void SetPlayerReference(class ASRPGCharacter* ref_);
	void SetPlayerReference_Implementation(class ASRPGCharacter* ref_);
	bool SetPlayerReference_Validate(class ASRPGCharacter* ref_);

	void CheckCollisionUnderMouse();

	void BeginPlay() override;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */
	UFUNCTION(Reliable, Client, WithValidation)
	void MoveToMouseCursor();
	void MoveToMouseCursor_Implementation();
	bool MoveToMouseCursor_Validate();


	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	UFUNCTION(Reliable, Server, WithValidation)
	void SetNewMoveDestination(const FVector DestLocation);
	void SetNewMoveDestination_Implementation(const FVector DestLocation);
	bool SetNewMoveDestination_Validate(const FVector DestLocation);

	FHitResult target;
	float radius;

	bool shouldMove;

	/** Input handlers for SetDestination action. */
	UFUNCTION(Reliable, Client, WithValidation)
	void OnSetDestinationPressed();
	void OnSetDestinationPressed_Implementation();
	bool OnSetDestinationPressed_Validate();
	UFUNCTION(Reliable, Client, WithValidation)
	void OnSetDestinationReleased();
	void OnSetDestinationReleased_Implementation();
	bool OnSetDestinationReleased_Validate();

	class ASRPGCharacter* player;
	IInteractable* interacting;
};


