// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CutsceneTableStruct.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct SRPG_API FCutsceneTableStruct : public FTableRowBase
{

	GENERATED_USTRUCT_BODY()
public:
	FCutsceneTableStruct()
	{

	};

	// character name
	UPROPERTY(BlueprintReadWrite)
	FString name;
	// what should they say
	UPROPERTY(BlueprintReadWrite)
	FString dialogue;
	// how fast should it display
	UPROPERTY(BlueprintReadWrite)
	float textSpeed;
	// model on the right
	UPROPERTY(BlueprintReadWrite)
	FString model1Name;
	// model on the left
	UPROPERTY(BlueprintReadWrite)
	FString model2Name;
	// how many lines are there total
	UPROPERTY(BlueprintReadWrite)
	int lineNum; 
	UPROPERTY(BlueprintReadWrite)
	FString modelAnim1;
	UPROPERTY(BlueprintReadWrite)
	FString modelAnim2;

};
