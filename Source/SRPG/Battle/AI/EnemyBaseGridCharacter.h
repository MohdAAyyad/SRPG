// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/GridCharacter.h"
#include "EnemyBaseGridCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API AEnemyBaseGridCharacter : public AGridCharacter
{
	GENERATED_BODY()
protected:
	AEnemyBaseGridCharacter();
	void BeginPlay() override;
	class AAIManager* aiManager;
	class ATile* targetTile;
	class APlayerGridCharacter* targetPlayer;
	class ACrowdItem* targetItem;
	class AGridManager* gridManager;
	UPROPERTY(EditAnywhere, Category = "Detection")
		class UBoxComponent* detectionRadius;

	class ABattleController* btlCtrl;
	class ABattleManager* btlManager;
	TArray<ACrowdItem*> crdItems;

	bool bWillMoveAgain; //Need to make sure that once we've collided with an item, we don't tell that AI Manager that we've finished moving but rather move towards the player


	UPROPERTY(EditAnywhere, Category = "Targeting")
		int attackRange; //PLACEHOLDER

	UFUNCTION()
		void DetectItem(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult &sweepResult_);

	UFUNCTION()
		void TakeItem(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult &sweepResult_);

	void FindTheClosestPlayer();
	void MoveToTheTileWithinRangeOfThisTile(ATile* tile_);
	void CheckIfWeHaveAnyTargetItems();

public:
	void SetManagers(AAIManager* ref_, AGridManager* gref_, ABattleManager* bref_);
	void PickMovementDestination();
	void EnableDetectionCollision();
	void ExecuteChosenAttack();
	virtual void Selected() override;
	virtual void NotSelected() override;
	virtual void ActivateWeaponAttack() override;
	void MoveAccordingToPath() override;
	void ItemIsUnreachable(); //Called by the marked item when it's not obtained within the same turn or is obtained by someone else
};
