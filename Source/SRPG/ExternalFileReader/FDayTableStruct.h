// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FDayTableStruct.generated.h"

USTRUCT(BlueprintType)
struct SRPG_API FDayTableStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
		int id;
	UPROPERTY(BlueprintReadWrite)
		int enemyLevel;
	UPROPERTY(BlueprintReadWrite)
		int moneyReward;
	UPROPERTY(BlueprintReadOnly)
		int shardsReward;
	UPROPERTY(BlueprintReadWrite)
		int maxNumOfPlayers;
	UPROPERTY(BlueprintReadWrite)
		int numOfEnemies;
	UPROPERTY(BlueprintReadWrite)
		int specialFight; //0 for regular battles 1 for bosses
	UPROPERTY(BlueprintReadWrite)
		int retryMoneyCompensation; //How much money the player gets when they are defeated on this day
	UPROPERTY(BlueprintReadWrite)
		int retryShardsCompensation;  //How much shards the player gets when they are defeated on this day

};
