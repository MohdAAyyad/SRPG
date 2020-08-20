// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EquippedFightersTableStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct SRPG_API FEquippedFightersTableStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	FEquippedFightersTableStruct()
	{

	}

	UPROPERTY(BlueprintReadWrite)
	int bpid; //EditAnywhere, BluePrintReadOnly //Used to know which BP corresponds to which fighter
	UPROPERTY(BlueprintReadWrite)
	FString name; //EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int hp;// EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int pip; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int atk; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int def; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int intl; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int spd; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int crit; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int agl; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int crd; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int weaponIndex; //The type of weapon this SFighter can equip.
	UPROPERTY(BlueprintReadWrite)
	int armorIndex; //The type of armor this SFighter can wear.
	UPROPERTY(BlueprintReadWrite)
	int equippedWeapon; //Used to get the weapon's stats from the equipment table
	UPROPERTY(BlueprintReadWrite)
	int equippedArmor; //Used to get the armor's stats from the equipment table
	UPROPERTY(BlueprintReadWrite)
	int equippedAccessory; //Used to get the accessory's stats from the equipment table
	UPROPERTY(BlueprintReadWrite)
	int level;
	UPROPERTY(BlueprintReadWrite)
	int currentEXP;
	UPROPERTY(BlueprintReadWrite)
	int neededEXPToLevelUp;
	UPROPERTY(BlueprintReadWrite)
	int emitterIndex; //For regular attacks
	UPROPERTY(BlueprintReadWrite)
	int id;
	UPROPERTY(BlueprintReadWrite)
	int archetype;
};
