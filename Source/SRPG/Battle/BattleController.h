// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattleController.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API ABattleController : public APlayerController
{
	GENERATED_BODY()

public:
	ABattleController();
	void SetBattlePawn(class ABattlePawn* pawn_);
	void SetBattleManager(class ABattleManager* btlMaanger_);

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Navigate player to the given world location. */
	float SetNewMoveDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void HandleMousePress();

	class AGridCharacter* controlledCharacter;
	ABattlePawn* battlePawn;
	class ATile* targetTile;
	ABattleManager* btlManager;

	//Deployment
	bool bReDeployingUnit; //Used to know whether the player wants to change the position of a deployed unit

	//Skill targeting
	bool bTargetingWithASkill;
	int targetingRowSpeed;
	int targetingDepthSpeed;
	int targetingPure;
	TArray<ATile*> targetingTiles;
	TWeakObjectPtr<AActor,FWeakObjectPtr> previosulyTargetedActor;

	void TargetingWithASkill();
	void CancelCommand();

public:
	void FocusOnGridCharacter(AGridCharacter* chr_, float rate_);
	void SetTargetingWithSkill(bool value_,int row_, int depth_, int pure_);
	void ResetViewLock();
	UFUNCTION(BlueprintCallable)
		void ResetFocus();
	void ResetControlledCharacter();

	float focusRate;
};
