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
};
