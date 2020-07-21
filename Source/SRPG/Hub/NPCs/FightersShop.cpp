// Fill out your copyright notice in the Description page of Project Settings.


#include "FightersShop.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"

void AFightersShop::BeginPlay()
{
	Super::BeginPlay();

}

void AFightersShop::EndDialogue()
{
	if (widget)
	{
		widget->GetUserWidgetObject()->RemoveFromViewport();
	}
}

TArray<FFighter> AFightersShop::GetAllFightersForSale()
{
	TArray<FFighter> fighters;
	// loop three times
	for (int i = 1; i >= 3; i++)
	{
		FFighter fighter;
		FString rowName = FString::FromInt(i);
		FName converted = FName(*rowName);
		// All fighters will be in index 0
		// Equpipped fighter will be in index 1
		FFighterTableStruct row = fileReader->FindFighterTableRow(converted, 0);
		// load in all our values
		fighter.BPID = row.bpid;
		fighter.name = row.name;
		fighter.level = row.level;
		fighter.currentEXP = row.currentEXP;
		fighter.neededEXPToLevelUp = row.neededEXPToLevelUp;
		fighter.weaponIndex = row.weaponIndex;
		fighter.armorIndex = row.armorIndex;
		fighter.equippedWeapon = row.equippedWeapon;
		fighter.equippedArmor = row.equippedArmor;
		fighter.equippedAccessory = row.equippedAccessory;
		fighter.emitterIndex = row.emitterIndex;
		//add the fighter to the array
		fighters.Push(fighter);
	}
	return fighters;
}

void AFightersShop::ChooseFighter(int fighterIndex_)
{
	FName index = FName(*FString::FromInt(fighterIndex_));

	FFighterTableStruct row = fileReader->FindFighterTableRow(index, 0);
	FFighter fighter;
	fighter.BPID = row.bpid;
	fighter.name = row.name;
	fighter.level = row.level;
	fighter.currentEXP = row.currentEXP;
	fighter.neededEXPToLevelUp = row.neededEXPToLevelUp;
	fighter.weaponIndex = row.weaponIndex;
	fighter.armorIndex = row.armorIndex;
	fighter.equippedWeapon = row.equippedWeapon;
	fighter.equippedArmor = row.equippedArmor;
	fighter.equippedAccessory = row.equippedAccessory;
	fighter.emitterIndex = row.emitterIndex;

	chosenFighter = fighter;
}

void AFightersShop::UpdateName(FString name_)
{
	chosenFighter.name = name_;
}

TArray<int> AFightersShop::LevelUpFighter()
{
	return TArray<int>();
}

TArray<int> AFightersShop::LevelDownFighter()
{
	return TArray<int>();
}

void AFightersShop::FinalizePurchase()
{
	// load all of the chosen fighters values into the data table;
	FFighterTableStruct row;
	row.bpid = chosenFighter.BPID;
	row.name = chosenFighter.name;
	row.level = chosenFighter.level;
	row.currentEXP = chosenFighter.currentEXP;
	row.neededEXPToLevelUp = chosenFighter.neededEXPToLevelUp;
	row.weaponIndex = chosenFighter.weaponIndex;
	row.armorIndex = chosenFighter.armorIndex;
	row.equippedWeapon = chosenFighter.equippedWeapon;
	row.equippedArmor = chosenFighter.equippedArmor;
	row.equippedAccessory = chosenFighter.equippedAccessory;
	row.emitterIndex = chosenFighter.emitterIndex;

	// add the new row to the table
	FName index = FName(*FString::FromInt(purchasedFighters));
	fileReader->AddRowToFighterTable(index, 1, row);
}
