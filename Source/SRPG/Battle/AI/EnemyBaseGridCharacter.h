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
	AGridCharacter* targetCharacter;
	class ACrowdItem* targetItem;
	class AGridManager* gridManager;
	UPROPERTY(EditAnywhere, Category = "Detection")
		class UBoxComponent* detectionRadius;

	class ABattleManager* btlManager;
	TArray<ACrowdItem*> crdItems;

	bool bWillMoveAgain; //Need to make sure that once we've collided with an item, we don't tell that AI Manager that we've finished moving but rather move towards the player
	bool bCannotFindTile; //Sometimes GetMyTile() will return nullptr when the character calls the function while transitioning from one tile to the next. This bool is used to call GetMyTile() on tick until we find a tile (i.e the character will not stop moving) --> Necessary when obtaining items as the enemy might obtain the item while in between tiles
	
	UPROPERTY(EditAnywhere, Category = "Equipment")
		int weaponIndex;
	UPROPERTY(EditAnywhere, Category = "Equipment")
		int armorIndex;
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

	void MoveToTheTileWithinRangeOfThisTile(ATile* startingTile_,ATile* targetTile_);
	void CheckIfWeHaveAnyTargetItems();
	void AddEquipmentStats(int tableIndex_) override;

	void Tick(float DeltaTime) override;

	void ResetCameraFocus() override;

	UPROPERTY(EditAnywhere, Category = "Patterns")
		bool bHealer;
	UPROPERTY(EditAnywhere, Category = "Patterns")
		class UDecisionComp* decisionComp;

public:
	void SetManagers(AAIManager* ref_, AGridManager* gref_, ABattleManager* bref_);
	void MoveCloserToTargetPlayer(ATile* startingTile_);
	void StartEnemyTurn();
	void ExecuteChosenAttack();
	virtual void Selected() override;
	virtual void NotSelected() override;
	virtual void ActivateWeaponAttack() override;
	void MoveAccordingToPath() override;
	void ItemIsUnreachable(ATile* startingTile_); //Called by the marked item when it's not obtained within the same turn or is obtained by someone else
	AGridCharacter* GetCurrentTarget();
};
