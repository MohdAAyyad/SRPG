// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/AI/DecisionComp.h"
#include "BlankDecisionComp.generated.h"

UENUM()
enum class ETargetConditions : uint8
{
	OFFENSE_SMALLEST_DISTANCE = 0,
	OFFENSE_LONGEST_DISTANCE = 1,
	OFFENSE_LOWEST_CONDITIONED_STAT = 2,
	OFFENSE_HIGHEST_CONDITIONED_STAT = 3,

	SUPPORT_SMALLEST_DISTANCE = 4,
	SUPPORT_LONGEST_DISTANCE = 5,
	SUPPORT_LOWEST_CONDITIONED_STAT = 6,
	SUPPORT_HIGHEST_CONDITIONED_STAT = 7
};

UCLASS()
class SRPG_API UBlankDecisionComp : public UDecisionComp
{
	GENERATED_BODY()

public:
	UBlankDecisionComp();

	AGridCharacter* FindTheOptimalTargetCharacter() override;
	class ATile* FindOptimalTargetTile(ATile* myTile_) override;

	AGridCharacter* FindNextOptimalTargetCharacter(AGridCharacter* ignoreThis_); //Used in case we don't find the first target and we need to change targets

	void ResetFunctionIndex() override;
	//Function pointers
	typedef AGridCharacter* (UBlankDecisionComp::*findTargetFuncPtr)(AGridCharacter*); //Takes gridcharacter and returns gridcharacter
	typedef ATile* (UBlankDecisionComp::*findTileFuncPtr)(ATile*); //Takes tile and returns tile
protected:

	void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Target radius in tiles. Must be larger than zero to take effect.")
		uint32 offenseTargetRadiusInTiles;
	UPROPERTY(EditAnywhere, Category = "Target radius in tiles. Must be larger than zero to take effect.")
		uint32 supportTargetRadiusInTiles;
	UPROPERTY(EditAnywhere, Category = "Condition stat index. Only applicable when using a suitable condition")
		uint32 offenseConditionStatIndex;
	UPROPERTY(EditAnywhere, Category = "Condition stat index. Only applicable when using a suitable condition")
		TArray<uint32> supportPriorityConditionStatIndex;

	UPROPERTY(EditAnywhere, Category = "Conditions")
		float hpThresholdForSupport;
	UPROPERTY(EditAnywhere, Category = "Conditions")
		float pipThresholdForSupport;

	UPROPERTY(EditAnywhere, Category = "Conditions")
		TArray<ETargetConditions> conditions;



	TArray<findTargetFuncPtr> targetFunctionPtrs;
	TArray<findTileFuncPtr> tileFunctionPtrs;

	float offenseTargetRadiusInPx;
	float supportTargetRadiusInPx;

	int functionIndex; //Used to call the correct functions in the target and tile function ptrs arrays;


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
	ATile* FindOptimalDistanceBasedSupportTile(ATile* myTile_); //Checks conditioned stat here
	ATile* FindOptimalStatBasedSupportTile(ATile* myTile_); //Already picked the character based on the stat, now just going to pick a tile

	
};
