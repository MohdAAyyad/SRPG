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
		id = value = statIndex = owned = level = price = 0;
		name = description = "";
	}
	UPROPERTY(BlueprintReadWrite)
		int id;
	UPROPERTY(BlueprintReadWrite)
		FString name;
	UPROPERTY(BlueprintReadWrite)
		FString description;
	UPROPERTY(BlueprintReadWrite)
		int value; //How much they affect the stat
	UPROPERTY(BlueprintReadWrite)
		int statIndex;
	UPROPERTY(BlueprintReadWrite)
		int owned; //How many we own.
	UPROPERTY(BlueprintReadWrite)
		int level; //Used to determine whether this item can be sold or not. Must be less or equal to world level
	UPROPERTY(BlueprintReadWrite)
		int price;
};
