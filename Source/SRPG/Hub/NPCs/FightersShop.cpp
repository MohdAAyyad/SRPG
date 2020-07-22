// Fill out your copyright notice in the Description page of Project Settings.


#include "FightersShop.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"

void AFightersShop::BeginPlay()
{
	Super::BeginPlay();
	purchasedFighters = 1;
	haveChosenFighter = false;
	hasLeveledUp = false;
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
	if (fileReader->GetTable(0))
	{
		rowNames = fileReader->GetTable(0)->GetRowNames();

		for (auto n : rowNames)
		{
			FFighter fighter;
			// All fighters will be in index 0
			// Equpipped fighter will be in index 1
			FFighterTableStruct row = fileReader->FindFighterTableRow(n, 0);
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
			// load in all our values

			//add the fighter to the array
			fighters.Push(fighter);
		}
		return fighters;
	}
	return TArray<FFighter>();
}

void AFightersShop::ChooseFighter(int fighterIndex_)
{
	TArray<FFighter> fighters = GetAllFightersForSale();
	//chosenFighter = fighters[fighterIndex_];
	chosenFighter.BPID = fighters[fighterIndex_].BPID;
	chosenFighter.name = fighters[fighterIndex_].name;
	chosenFighter.level = fighters[fighterIndex_].level;
	chosenFighter.currentEXP = fighters[fighterIndex_].currentEXP;
	chosenFighter.neededEXPToLevelUp = fighters[fighterIndex_].neededEXPToLevelUp;
	chosenFighter.weaponIndex = fighters[fighterIndex_].weaponIndex;
	chosenFighter.armorIndex = fighters[fighterIndex_].armorIndex;
	chosenFighter.equippedWeapon = fighters[fighterIndex_].equippedWeapon;
	chosenFighter.equippedArmor = fighters[fighterIndex_].equippedArmor;
	chosenFighter.equippedAccessory = fighters[fighterIndex_].equippedAccessory;
	chosenFighter.emitterIndex = fighters[fighterIndex_].emitterIndex;
	UE_LOG(LogTemp, Warning, TEXT("Choose Fighter Called!"));
	currentIndex = fighterIndex_;
	haveChosenFighter = true;
	//fighters[]
	//FFighter fighter;
	//fighter.BPID = row.bpid;
	//fighter.name = row.name;
	//fighter.level = row.level;
	//fighter.currentEXP = row.currentEXP;
	//fighter.neededEXPToLevelUp = row.neededEXPToLevelUp;
	//fighter.weaponIndex = row.weaponIndex;
	//fighter.armorIndex = row.armorIndex;
	//fighter.equippedWeapon = row.equippedWeapon;
	//fighter.equippedArmor = row.equippedArmor;
	//fighter.equippedAccessory = row.equippedAccessory;
	//fighter.emitterIndex = row.emitterIndex;


}

void AFightersShop::UpdateName(FString name_)
{
	chosenFighter.name = name_;
}

FString AFightersShop::GetFighterInfo(int fighterIndex_)
{
	FName converted = FName(*FString::FromInt(fighterIndex_));
	FFighterTableStruct row = fileReader->FindFighterTableRow(converted, 0);

	return FString();
}


FString AFightersShop::PrintFighter1()
{
	GetAllFightersForSale();
	FFighterTableStruct row = fileReader->FindFighterTableRow(rowNames[0], 0);
	// load all of the stat values and print them to a string
	return FString("HP: " + FString::FromInt(row.hp) + " PIP: "+ " ATK: " + FString::FromInt(row.atk) + " DEF: " + FString::FromInt(row.def) + " INT: " + FString::FromInt(row.intl) + " SPD: " + FString::FromInt(row.spd) + " CRIT: " + FString::FromInt(row.crit) + " HIT: " + FString::FromInt(row.hit) + " CRD: " + FString::FromInt(row.crd));
}

FString AFightersShop::PrintFighter2()
{
	GetAllFightersForSale();
	//return FString("This is where the relevant stats for fighter 2 is displyed");
	FFighterTableStruct row = fileReader->FindFighterTableRow(rowNames[1], 0);
	// load all of the stat values and print them to a string
	return FString("HP: " + FString::FromInt(row.hp) + " PIP: " + " ATK: " + FString::FromInt(row.atk) + " DEF: " + FString::FromInt(row.def) + " INT: " + FString::FromInt(row.intl) + " SPD: " + FString::FromInt(row.spd) + " CRIT: " + FString::FromInt(row.crit) + " HIT: " + FString::FromInt(row.hit) + " CRD: " + FString::FromInt(row.crd));

}

FString AFightersShop::PrintFighter3()
{
	//pasting this three times is really inefficient but I'll change it later
	GetAllFightersForSale();
	//return FString("This is where the relevant stats for fighter 3 is displyed");
	FFighterTableStruct row = fileReader->FindFighterTableRow(rowNames[2], 0);
	// load all of the stat values and print them to a string
	return FString("HP: " + FString::FromInt(row.hp) + " PIP: " + " ATK: " + FString::FromInt(row.atk) + " DEF: " + FString::FromInt(row.def) + " INT: " + FString::FromInt(row.intl) + " SPD: " + FString::FromInt(row.spd) + " CRIT: " + FString::FromInt(row.crit) + " HIT: " + FString::FromInt(row.hit) + " CRD: " + FString::FromInt(row.crd));
}

TArray<int> AFightersShop::LevelUpFighter()
{
	if (haveChosenFighter)
	{
		TArray<int> tempStats;
		FFighterTableStruct row = fileReader->FindFighterTableRow(rowNames[currentIndex], 0);
		tempStats.Push(row.hp); // 0
		tempStats.Push(row.pip); // 1
		tempStats.Push(row.atk); // 2
		tempStats.Push(row.def); // 3
		tempStats.Push(row.intl); // 4
		tempStats.Push(row.spd); // 5
		tempStats.Push(row.crit); // 6
		tempStats.Push(row.hit); // 7
		tempStats.Push(row.crd); // 8
		tempStats.Push(row.level); // 9

		tempStats = chosenFighter.LevelUpUntilGoal(tempStats[9] + 1, tempStats);
		UE_LOG(LogTemp, Warning, TEXT("Leveled Up Fighter"));
		statsAfterLevelUp = tempStats;
		hasLeveledUp = true;
		return tempStats;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FIGHTER NOT SELECTED!"));
		return TArray<int>();
	}

}

TArray<int> AFightersShop::LevelDownFighter()
{
	if (haveChosenFighter)
	{
		TArray<int> tempStats;
		FFighterTableStruct row = fileReader->FindFighterTableRow(rowNames[currentIndex], 0);
		tempStats.Push(row.hp); // 0
		tempStats.Push(row.pip); // 1
		tempStats.Push(row.atk); // 2
		tempStats.Push(row.def); // 3
		tempStats.Push(row.intl); // 4
		tempStats.Push(row.spd); // 5
		tempStats.Push(row.crit); // 6
		tempStats.Push(row.hit); // 7
		tempStats.Push(row.crd); // 8
		tempStats.Push(row.level); // 9

		tempStats = chosenFighter.LevelUpUntilGoal(tempStats[9] - 1, tempStats);
		UE_LOG(LogTemp, Warning, TEXT("Leveled Up Fighter"));
		statsAfterLevelUp = tempStats;
		hasLeveledUp = true;
		return tempStats;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FIGHTER NOT SELECTED!"));
		return TArray<int>();
	}

}

void AFightersShop::FinalizePurchase()
{
	// load all of the chosen fighters values into the data table;
	if (haveChosenFighter)
	{
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
		// load the data not included in the FFighter Struct
		FFighterTableStruct row2 = fileReader->FindFighterTableRow(rowNames[currentIndex], 0);
		row.hp = row2.hp;
		row.pip = row2.pip;
		row.atk = row2.atk;
		row.def = row2.def;
		row.intl = row2.intl;
		row.spd = row2.spd;
		row.crit = row2.crit;
		row.hit = row2.hit;
		row.crd = row2.crd;
		row.id = row2.id;
		if (hasLeveledUp)
		{
			row.hp = statsAfterLevelUp[0];
			row.pip = statsAfterLevelUp[1];
			row.atk = statsAfterLevelUp[2];
			row.def = statsAfterLevelUp[3];
			row.intl = statsAfterLevelUp[4];
			row.spd = statsAfterLevelUp[5];
			row.crit = statsAfterLevelUp[6];
			row.hit = statsAfterLevelUp[7];
			row.crd = statsAfterLevelUp[8];
		}

		hasLeveledUp = false;
		// add the new row to the table
		FName index = FName(*FString::FromInt(purchasedFighters));
		fileReader->AddRowToFighterTable(index, 1, row);
		purchasedFighters += 1;
		UE_LOG(LogTemp, Warning, TEXT("Finalize Purchase"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Fighter Selected!"));
	}
	
}

void AFightersShop::LoadText()
{
	// empty function because we don't want the values on a dialogue table to be loaded
}
