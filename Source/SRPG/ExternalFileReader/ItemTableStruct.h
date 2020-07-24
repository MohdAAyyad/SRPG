// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemTableStruct.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct SRPG_API FItemTableStruct :  public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FItemTableStruct()
	{

	}
	UPROPERTY(BlueprintReadWrite)
		int id; //EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
		FString name; //EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
		int value;  //EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
		int statIndex; //EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
		int owned; //How many we own.
	UPROPERTY(BlueprintReadWrite)
		int level; //Used to determine whether this item can be sold or not. Must be less or equal to world level
};
