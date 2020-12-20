// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../ExternalFileReader/FSkillTableStruct.h"
#include "DecisionComp.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SRPG_API UDecisionComp : public UActorComponent
{
	GENERATED_BODY()

protected:
	bool bTargetIsHostile; //True for players, false for enemies. Changes based on what the pattern decides is the target
	class AAIManager* aiManager;
	class ABattleManager* btlManager;
	class AEnemyBaseGridCharacter* ownerEnemy;
	class AGridCharacter* currentTarget;


	UPROPERTY(BlueprintReadOnly, Category = "Skills")
		TArray<FSkillTableStruct> offsenseSkills;
	UPROPERTY(BlueprintReadOnly, Category = "Skills")
		TArray<FSkillTableStruct> defenseSkills;
	
	int offenseSkillWithTheMaxRangeIndex;
	int defenseSkillWithTheMaxRangeIndex;

	bool bCanUseSkill; //Affected by remaning pips
	bool bWillUseSkill; //True if the enemy is gonna be using a skill
	int skillType; //-1 if enemy will not use a skill, 0 if offense skill, and 1 if defense skill
	UPROPERTY(EditAnywhere, Category = "Skill chance. Will be affected by archetype")
	int skillChance; //Random number affected by archetype

	//Items
	TArray<class ACrowdItem*> crdItems;

	UPROPERTY(EditAnywhere, Category = "Persistence")
		bool bPersistant; //Persistant enemies will not change targets and will always attemp to kill their target before moving on to the next one

public:	
	UDecisionComp();
	virtual AGridCharacter* FindTheOptimalTargetCharacter();
	virtual class ATile* FindOptimalTargetTile(ATile* myTile_);
	void SetRefs(AAIManager* ai_, ABattleManager* btl_, AEnemyBaseGridCharacter* enemy_);
	
	FSkillTableStruct GetChosenSkill();
	bool GetWillUseSkill(int& skillType_);
	void ResetCurrentTarget();

	//Items

	ACrowdItem* UpdateTargetItem(class AGridManager* grid_, ATile* originTile_, int rows_, int depths_);
	void AddCrdItem(ACrowdItem* newItem_);
	void RemoveCrdItem(ACrowdItem* newItem_);
	void ClearCrdItems();

	void SetPersistence(bool value_);

	virtual void ResetFunctionIndex() {}; //used by blank enemies only

protected:
	virtual void BeginPlay() override;

	void UpdateEnemySkills();
	virtual ATile* ChooseTileBasedOnPossibleOffenseActions(ATile* myTile_); //Checks the ranges of the skills and the regular attack and sees which is more viable to be used
	virtual ATile* ChooseTileBasedOnPossibleSupportActions(ATile* myTile_);

	void FindDefensiveSkillThatUpdatesThisStatAndHasTheHighestRange(int statIndex_, int currentPips_);
	void PickTheNextUsableSkill(class UStatsComponent* statsComp_, bool offense_);


	bool CheckIfTargetIsInRangeOfSkill(AGridManager* grid_, class UPathComponent*path_,
									   TArray<ATile*>& movementTiles, TArray<ATile*>& rangeTiles_, ATile** myTile_, ATile** resultTile_, bool offense_);


	bool CheckIfPlayerIsInRangeOfRegularAttack(AGridManager* grid_, UStatsComponent* statsComp_, class UPathComponent*path_,
												TArray<ATile*>& movementTiles, TArray<ATile*>& rangeTiles_, ATile** myTile_, ATile** resultTile_);

	void PickAttackOrSkillBasedOnLeastRange(AGridManager* grid_, UStatsComponent* statsComp_, class UPathComponent*path_,
		TArray<ATile*>& movementTiles, TArray<ATile*>& rangeTiles_, ATile** myTile_, ATile** resultTile_);

	bool CheckIfINeedThisItem(UStatsComponent* stats_, int statIndex_, int itemValue_);

		
};
