// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../ExternalFileReader/FSkillTableStruct.h"
#include "DecisionComp.generated.h"

UENUM()
enum EPatterns
{
	DEFAULT = 0, //Gets close to a player and attacks
	ASSASSIN = 1, //Picks the player with the lowest health within its range
	FOLLOWER = 2, //Picks an enemy close by and follows them. Targets the same enemy they're targeting. Moves the same as DEFAULT if a target has not been selected yet.
	PEOPLEPERSON = 3, //Gets a buff when fellow enemies are close by and a nerf when not. Moves the same way as DEFAULT.
	SUPPORT = 4 //Uses buff and heal skills on fellow enemies
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SRPG_API UDecisionComp : public UActorComponent
{
	GENERATED_BODY()

protected:
	EPatterns currentPattern;
	bool bTargetIsHostile; //True for players, false for enemies. Changes based on what the pattern decides is the target
	class AAIManager* aiManager;
	class ABattleManager* btlManager;
	class AEnemyBaseGridCharacter* ownerEnemy;
	class AGridCharacter* currentTarget;
	float targetRadius; //Used by all the patterns besides Default. Targets need to be within this radius to be considered


	UPROPERTY(BlueprintReadOnly, Category = "Skills")
		TArray<FSkillTableStruct> offsenseSkills;
	UPROPERTY(BlueprintReadOnly, Category = "Skills")
		TArray<FSkillTableStruct> defenseSkills;
	
	int offenseSkillWithTheMaxRangeIndex;
	int defenseSkillWithTheMaxRangeIndex;

	bool bCanUseSkill; //Affected by remaning pips
	bool bWillUseSkill; //True if the enemy is gonna be using a skill
	int skillType; //-1 if enemy will not use a skill, 0 if offense skill, and 1 if defense skill
	int skillChance; //Random number affected by archetype

public:	
	UDecisionComp();
	AGridCharacter* FindTheOptimalTargetCharacter();
	class ATile* FindOptimalTargetTile(ATile* myTile_);
	void UpdatePattrn(int level_, bool healer_); // Certain patterns are level gated. Hearlers are predetermined
	void SetRefs(AAIManager* ai_, ABattleManager* btl_, AEnemyBaseGridCharacter* enemy_);
	bool IsMyTargetHostile(); //Depends on the pattern. Used by enemies to know if they should use offensive or support skills
	
	AGridCharacter* FindAnotherTarget(AGridCharacter* target_);

	FSkillTableStruct GetChosenSkill();
	bool GetWillUseSkill(int& skillType_);
	void ResetCurrentTarget();
protected:
	virtual void BeginPlay() override;

	AGridCharacter* FindDefaultTarget();
	AGridCharacter* FindDefaultTarget(TArray<APlayerGridCharacter*> deployedPlayers_,FVector myLoc_);
	AGridCharacter* FindAssassinTarget();
	AGridCharacter* FindFollowerTarget();
	AGridCharacter* PeoplePersonEffect();
	AGridCharacter* FindHealTarget();
	AGridCharacter* FindSupportTarget(TArray<AEnemyBaseGridCharacter*> deployedEnemies_,FVector myLoc_);
	AGridCharacter* FindClosestAllyWithTheLeastHealth(TArray<AEnemyBaseGridCharacter*> echars, FVector myLoc);


	void UpdateEnemySkills();
	ATile* FindDefaultTile(ATile* myTile_);  //Used by most patterns
	ATile* FindSupportTile(ATile* myTile_);
	ATile* ChooseTileBasedOnPossibleOffenseActions(ATile* myTile_); //Checks the ranges of the skills and the regular attack and sees which is more viable to be used
	ATile* ChooseTileBasedOnPossibleSupportActions(ATile* myTile_);

	void FindDefensiveSkillThatUpdatesThisStatAndHasTheHighestRange(int statIndex_, int currentPips_);
	void PickTheNextUsableSkill(class UStatsComponent* statsComp_, bool offense_);


	bool CheckIfTargetIsInRangeOfSkill(class AGridManager* grid_, class UPathComponent*path_,
									   TArray<ATile*>& movementTiles, TArray<ATile*>& rangeTiles_, ATile** myTile_, ATile** resultTile_, bool offense_);


	bool CheckIfPlayerIsInRangeOfRegularAttack(class AGridManager* grid_, UStatsComponent* statsComp_, class UPathComponent*path_,
												TArray<ATile*>& movementTiles, TArray<ATile*>& rangeTiles_, ATile** myTile_, ATile** resultTile_);

	void PickAttackOrSkillBasedOnLeastRange(class AGridManager* grid_, UStatsComponent* statsComp_, class UPathComponent*path_,
		TArray<ATile*>& movementTiles, TArray<ATile*>& rangeTiles_, ATile** myTile_, ATile** resultTile_);


public:	
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
