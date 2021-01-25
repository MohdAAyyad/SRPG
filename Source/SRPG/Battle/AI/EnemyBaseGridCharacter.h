// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/GridCharacter.h"
#include "EnemyBaseGridCharacter.generated.h"

UENUM()
enum class EDecisionTrees : uint8
{
	DISTANCEBASED = 0, //Gets close to a player and attacks
	ASSASSIN = 1, //Picks the player with the lowest health within its range
	FOLLOWER = 2, //Picks an enemy close by and follows them. Targets the same enemy they're targeting. Moves the same as DEFAULT if a target has not been selected yet.
	PEOPLEPERSON = 3, //Gets a buff when fellow enemies are close by and a nerf when not. Moves the same way as DEFAULT.
	SUPPORT = 4, //Uses buff and heal skills on fellow enemies
	LEVELBASED =5 //Used the level of the protagonist to determine the tree
};

UCLASS()
class SRPG_API AEnemyBaseGridCharacter : public AGridCharacter
{
	GENERATED_BODY()
protected:
	AEnemyBaseGridCharacter();
	void BeginPlay() override;
	UPROPERTY(BlueprintReadOnly, Category = "AIManager")
		class AAIManager* aiManager;
	class ATile* targetTile;
	AGridCharacter* targetCharacter;
	class ACrowdItem* targetItem;
	class AGridManager* gridManager;
	UPROPERTY(EditAnywhere, Category = "Detection")
		class UBoxComponent* detectionRadius;

	bool bWillMoveAgain; //Need to make sure that once we've collided with an item, we don't tell that AI Manager that we've finished moving but rather move towards the player
	bool bCannotFindTile; //Sometimes GetMyTile() will return nullptr when the character calls the function while transitioning from one tile to the next. This bool is used to call GetMyTile() on tick until we find a tile (i.e the character will not stop moving) --> Necessary when obtaining items as the enemy might obtain the item while in between tiles
	
	UPROPERTY(EditAnywhere, Category = "Equipment")
		int weaponIndex;
	UPROPERTY(EditAnywhere, Category = "Equipment")
		int armorIndex;
	UPROPERTY(EditAnywhere, Category = "Equipment")
		int speed;
	UPROPERTY(EditAnywhere, Category = "Decision Trees")
		EDecisionTrees tree;
	UPROPERTY(EditAnywhere, Category = "Decision Trees")
		bool bPersistent;

public:
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

protected:
	void MoveToTheTileWithinRangeOfThisTile(ATile* startingTile_,ATile* targetTile_);
	virtual void AddEquipmentStats(int tableIndex_) override;

	void Tick(float DeltaTime) override;

	void ResetCameraFocus() override;

	void ChooseDecisionTreeBasedBasedOnLevel();

	UPROPERTY(EditAnywhere, Category = "Patterns")
		class UDecisionComp* decisionComp;

	bool bLookForANewTargetMidAttack; //In case the current target is dead then look for another target in your attack phase

	bool bHealer;

	virtual void EndTurn();

public:
	virtual void SetManagers(AAIManager* ref_, AGridManager* gref_, ABattleManager* bref_, ABattleCrowd* cref_);
	void MoveCloserToTargetPlayer(ATile* startingTile_);
	virtual void StartEnemyTurn();
	void ExecuteChosenAction();
	virtual void Selected() override;
	virtual void NotSelected() override;
	virtual void ActivateWeaponAttack() override;
	void MoveAccordingToPath() override;
	void ItemIsUnreachable(ATile* startingTile_); //Called by the marked item when it's not obtained within the same turn or is obtained by someone else
	AGridCharacter* GetCurrentTarget();

	void GridCharTakeDamage(float damage_, AGridCharacter* attacker_, bool crit_, int statusEffect_) override;
	void GridCharReactToSkill(float value_, int statIndex_, int statuEffectIndex_, AGridCharacter* attacker_, bool crit_) override;
	void GridCharReatToElemental(float damage_, int statusEffectIndex_) override;

	void ActivateSkillAttack() override;

	void CheckChangeStats();

	void IamDeadStopTargetingMe() override;

	void UpdateTargetCharacter(AGridCharacter* newTarget_); //Called by the decision component upon changing targets

	void ActivateOutline(bool value_) override;
};
