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
	int BPID;
	//name of the fighter
	FString name;
	// short description of the character
	FString description;
	// fighters level
	int level;
	// level scale
	int levelScale;
	//what sub archetype to use
	int subArchetype;
	// how much exp does this character have by default
	int currentEXP;
	// how much do they need to level up
	int neededEXPToLevelUp;

	UPROPERTY(BlueprintReadOnly)
		int price;

	//determines which weapon they use
	int weaponIndex;
	//determines which armor they can wear
	int armorIndex;
	//get the weapon stats from the weapon table
	int equippedWeapon;
	//get the armor stats from the armor table
	int equippedArmor;
	// get the accessory stats from the accessory table
	int equippedAccessory;

	// used for regular attacks
	int emitterIndex;

	void CalculatePrice();

	void ScaleStatsByLevel(int newLevel_);

	void LevelUpUntilGoal(int goalLevel_);
};
