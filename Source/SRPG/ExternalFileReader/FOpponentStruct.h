// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Definitions.h"
#include "FOpponentStruct.generated.h"

USTRUCT(BlueprintType)
struct SRPG_API FOpponentStruct
{
	GENERATED_USTRUCT_BODY()

	FOpponentStruct()
	{

	}

	UPROPERTY(BlueprintReadOnly)
		int level; // based on protagonist level. Range protagonist level +1 to protagonist level + 3
	UPROPERTY(BlueprintReadOnly)
		int archtype; //Random
	UPROPERTY(BlueprintReadOnly)
		int numberOfTroops; //Random. Range between player current roster size to player current roster size + 8 (get it from intermediate). Must always be larger than level + 3.

	void InitStruct(int level_, int num_) //The level and num passed are obtained from the intemediate
	{
		archtype = FMath::RandRange(ARCH_ATK, ARCH_INT);
		int numChance = FMath::RandRange(0, 8);
		numberOfTroops = num_ + numChance;
		level = level_ + 2;
	}
};
