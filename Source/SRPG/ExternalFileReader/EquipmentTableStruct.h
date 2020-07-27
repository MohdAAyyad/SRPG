// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EquipmentTableStruct.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct SRPG_API FEquipmentTableStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FEquipmentTableStruct()
	{

	}
	UPROPERTY(BlueprintReadWrite)
	int hp; // EditAnywhere, BluePrintReadOnly
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
	int hit; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int crd; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int range; // EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	FString name;//EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadWrite)
	int equipmentIndex;//EditAnywhere, BluePrintReadOnly //0 weapon 1 armor 2 accessory
	UPROPERTY(BlueprintReadWrite)
	int weaponIndex;//EditAnywhere, BluePrintReadOnly //Only viable when this is a weapon. Determines what kind of weapon it is and who can equip it.
	UPROPERTY(BlueprintReadWrite)
	int armorIndex;//EditAnywhere, BluePrintReadOnly//Only viable when this is a piece of armor. Determines what kind of armor it is and who can wear it.
	UPROPERTY(BlueprintReadWrite)
	int owned; //How many pieces of that type of equipment we own.
	UPROPERTY(BlueprintReadWrite)
	int level; //Used to determine whether this item can be sold or not. Must be less or equal to world level.
};
