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

	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int bpid; //EditAnywhere, BluePrintReadOnly //Used to know which BP corresponds to which fighter
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString name; //EditAnywhere, BluePrintReadOnly
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int hp;// EditAnywhere, BluePrintReadOnly
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int pip; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int atk; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int def; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int intl; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int spd; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int crit; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int hit; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int crd; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int weaponIndex; //The type of weapon this SFighter can equip.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int armorIndex; //The type of armor this SFighter can wear.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int equippedWeapon; //Used to get the weapon's stats from the equipment table
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int equippedArmor; //Used to get the armor's stats from the equipment table
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int equippedAccessory; //Used to get the accessory's stats from the equipment table
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int level;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int currentEXP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int neededEXPToLevelUp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int emitterIndex; //For regular attacks
};
