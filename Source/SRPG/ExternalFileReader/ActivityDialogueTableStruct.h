// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ActivityDialogueTableStruct.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct SRPG_API FActivityDialogueTableStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FActivityDialogueTableStruct()
	{
		id = activityIndex = branchOrCentral = positiveOrNegative = unitCost = moneyCostPercentage = timeCost = startingDialogue =
			endDialogue = rewardMoneyPercent = rewardIndex = 0;
		dialogue = "";
	}
	UPROPERTY(BlueprintReadWrite)
	int id; //EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	FString dialogue;  //EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int activityIndex; //Which activity this is.
	UPROPERTY(BlueprintReadWrite)
	int branchOrCentral; //Who uses it 0 or 1
	UPROPERTY(BlueprintReadWrite)
	int positiveOrNegative; //Whether this dialogue reacts to a positive or a negative result
	UPROPERTY(BlueprintReadWrite)
	int unitCost; // how many units does this activity cost
	UPROPERTY(BlueprintReadWrite)
	int moneyCostPercentage; // how much money does this activity cost
	UPROPERTY(BlueprintReadWrite)
	int timeCost;
	UPROPERTY(BlueprintReadWrite)
	int startingDialogue; // 0 if not 1 if true
	UPROPERTY(BlueprintReadWrite)
	int endDialogue; // 0 if not, 1 if true. Unlike Negative Central, end dialogue is what appears if the player tries to talk after succeding or failing the task.
	UPROPERTY(BlueprintReadWrite)
	int rewardMoneyPercent;
	UPROPERTY(BlueprintReadWrite)
	int rewardIndex;
};
