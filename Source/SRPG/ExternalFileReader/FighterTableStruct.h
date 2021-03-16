// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FighterTableStruct.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct SRPG_API FFighterTableStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FFighterTableStruct()
	{
		bpid = hp = pip = atk = def = intl = spd = crit = agl = crd = weaponIndex = armorIndex = equippedWeapon = equippedAccessory =
			equippedArmor  = currentEXP = neededEXPToLevelUp = id = value = archetype = 0;
		name = description = "";
		level = 1;
	}

	UPROPERTY(BlueprintReadWrite)
	int bpid; //EditAnywhere, BluePrintReadOnly //Used to know which BP corresponds to which fighter
	UPROPERTY(BlueprintReadWrite)
	FString name; //EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadOnly)
	FString description;
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
	int id;
	UPROPERTY(BlueprintReadWrite)
	int value; // how much does this fighter cost to buy
	UPROPERTY(BlueprintReadWrite)
	int archetype;

	void ScaleStatsByLevelUp(int targetLevel_); //Same as the one found in stats component
	void ScaleStatsByLevelDown(int targetLevel_);
};
