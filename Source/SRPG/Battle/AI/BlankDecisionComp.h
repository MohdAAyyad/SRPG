// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/AI/DecisionComp.h"
#include "BlankDecisionComp.generated.h"

UENUM()
enum class ETargetConditions : uint8
{
	SMALLEST_DISTANCE = 0,
	LONGEST_DISTANCE = 1,
	LOWEST_CONDITIONED_STAT = 2,
	HIGHEST_CONDITIONED_STAT = 3
};

UCLASS()
class SRPG_API UBlankDecisionComp : public UDecisionComp
{
	GENERATED_BODY()

public:
	UBlankDecisionComp();

	AGridCharacter* FindTheOptimalTargetCharacter() override;
	class ATile* FindOptimalTargetTile(ATile* myTile_) override;

	//Function pointers
	typedef AGridCharacter* (UBlankDecisionComp::*findTargetFuncPtr)(AGridCharacter*); //Takes gridcharacter and returns gridcharacter
	typedef ATile* (UBlankDecisionComp::*findTileFuncPtr)(ATile*); //Takes tile and returns tile
protected:

	void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Target radius in tiles. Must be larger than zero to take effect.")
		uint32 offenseTargetRadiusInTiles;
	UPROPERTY(EditAnywhere, Category = "Target radius in tiles. Must be larger than zero to take effect.")
		uint32 supportTargetRadiusInTiles;
	UPROPERTY(EditAnywhere, Category = "Inclination. Must choose only one. Will only take effect its target radius counterpart is active")
		bool bInclinedToAttack;
	UPROPERTY(EditAnywhere, Category = "Inclination. Must choose only one. Will only take effect its target radius counterpart is active")
		bool bInclinedToSupport;
	UPROPERTY(EditAnywhere, Category = "Conditions")
		ETargetConditions offenseCondition;
	UPROPERTY(EditAnywhere, Category = "Conditions")
		ETargetConditions supportCondition;
	UPROPERTY(EditAnywhere, Category = "Condition stat index. Only applicable when using a suitable condition")
		uint32 offenseConditionStatIndex;
	UPROPERTY(EditAnywhere, Category = "Condition stat index. Only applicable when using a suitable condition")
		uint32 supportConditionStatIndex;

	float offenseTargetRadiusInPx;
	float supportTargetRadiusInPx;
	float offenseChance;
	bool bChosenToAttack;

	findTargetFuncPtr findOffenseTarget;
	findTargetFuncPtr findSupportTarget;

	findTileFuncPtr findOffenseTile;
	findTileFuncPtr findSupportTile;

	void UpdateInclination();
	void UpdateFunctionPointers();

	AGridCharacter* FindOffenseTarget_SmallestDistance(AGridCharacter* ignoreThis_);
	AGridCharacter* FindOffenseTarget_LongestDistance(AGridCharacter* ignoreThis_);
	AGridCharacter* FindOffenseTarget_LowestStat(AGridCharacter* ignoreThis_);
	AGridCharacter* FindOffenseTarget_HighestStat(AGridCharacter* ignoreThis_);

	AGridCharacter* FindSupportTarget_SmallestDistance(AGridCharacter* ignoreThis_);
	AGridCharacter* FindSupportTarget_LongestDistance(AGridCharacter* ignoreThis_);
	AGridCharacter* FindSupportTarget_LowestStat(AGridCharacter* ignoreThis_);
	AGridCharacter* FindSupportTarget_HighestStat(AGridCharacter* ignoreThis_);

	ATile* FindOptimalOffenseTile(ATile* myTile_);
	ATile* FindOptimalSupportTile(ATile* myTile_);


	
};
