// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "Fighter.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct SRPG_API FFighter
{
	GENERATED_USTRUCT_BODY()
public:
	FFighter()
	{

	}

	// reference the list of blueprints used by btl manager 
	UPROPERTY(BlueprintReadOnly)
	int BPID;
	//name of the fighter
	UPROPERTY(BlueprintReadOnly)
	FString name;
	// short description of the character
	UPROPERTY(BlueprintReadOnly)
	FString description;
	// fighters level
	UPROPERTY(BlueprintReadOnly)
	int level;
	// level scale
	UPROPERTY(BlueprintReadOnly)
	int levelScale;
	//what sub archetype to use
	UPROPERTY(BlueprintReadOnly)
	int subArchetype;
	// how much exp does this character have by default
	UPROPERTY(BlueprintReadOnly)
	int currentEXP;
	// how much do they need to level up
	UPROPERTY(BlueprintReadOnly)
	int neededEXPToLevelUp;

	UPROPERTY(BlueprintReadOnly)
		int price;

	//determines which weapon they use
	UPROPERTY(BlueprintReadOnly)
	int weaponIndex;
	//determines which armor they can wear
	UPROPERTY(BlueprintReadOnly)
	int armorIndex;
	//get the weapon stats from the weapon table
	UPROPERTY(BlueprintReadOnly)
	int equippedWeapon;
	//get the armor stats from the armor table
	UPROPERTY(BlueprintReadOnly)
	int equippedArmor;
	// get the accessory stats from the accessory table
	UPROPERTY(BlueprintReadOnly)
	int equippedAccessory;

	// used for regular attacks
	UPROPERTY(BlueprintReadOnly)
	int emitterIndex;


	void CalculatePrice();

	void ScaleStatsByLevel(int newLevel_);

	void LevelUpUntilGoal(int goalLevel_);
};
