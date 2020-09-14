// Fill out your copyright notice in the Description page of Project Settings.


#include "FightersShop.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Definitions.h"
#include "Intermediary/Intermediate.h"


AFightersShop::AFightersShop():ANPC()
{
	purchasedFighters = 1;
	haveChosenFighter = false;
	currentIndex = -1;
	fighterID = 0;
}
void AFightersShop::BeginPlay()
{
	Super::BeginPlay();
	fighterID = Intermediate::GetInstance()->GetLatestFighterID();
	GetAllFightersForSale();
}

void AFightersShop::EndDialogue()
{
	if (widget)
	{
		widget->GetUserWidgetObject()->RemoveFromViewport();
	}

	Intermediate::GetInstance()->SetLatestFighterID(fighterID);
}

TArray<FFighterTableStruct> AFightersShop::GetAllFightersForSale()
{
	TArray<FFighterTableStruct> fighters;
	// loop three times
	if (fileReader->GetTable(0))
	{
		rowNames = fileReader->GetTable(0)->GetRowNames();

		for (auto n : rowNames)
		{
			// All fighters will be in index 0
			// Equipped fighter will be in index 1
			FFighterTableStruct row = fileReader->FindFighterTableRow(n, 0);
			//add the fighter to the array
			fighters.Push(row);
		}
		return fighters;
	}
	return TArray<FFighterTableStruct>();
}

void AFightersShop::ChooseFighter(int fighterIndex_)
{
	// retrive all the info
	TArray<FFighterTableStruct> fighters = GetAllFightersForSale();
	//transfer data
	if (fighterIndex_ >= 0 && fighterIndex_ < fighters.Num())
	{
		chosenFighter.bpid = fighters[fighterIndex_].bpid;
		chosenFighter.name = fighters[fighterIndex_].name;
		chosenFighter.hp = fighters[fighterIndex_].hp;
		chosenFighter.pip = fighters[fighterIndex_].pip;
		chosenFighter.atk = fighters[fighterIndex_].atk;
		chosenFighter.def = fighters[fighterIndex_].def;
		chosenFighter.intl = fighters[fighterIndex_].intl;
		chosenFighter.spd = fighters[fighterIndex_].spd;
		chosenFighter.crit = fighters[fighterIndex_].crit;
		chosenFighter.agl = fighters[fighterIndex_].agl;
		chosenFighter.crd = fighters[fighterIndex_].crd;
		chosenFighter.level = fighters[fighterIndex_].level;
		chosenFighter.currentEXP = fighters[fighterIndex_].currentEXP;
		chosenFighter.neededEXPToLevelUp = fighters[fighterIndex_].neededEXPToLevelUp;
		chosenFighter.weaponIndex = fighters[fighterIndex_].weaponIndex;
		chosenFighter.armorIndex = fighters[fighterIndex_].armorIndex;
		chosenFighter.equippedWeapon = fighters[fighterIndex_].equippedWeapon;
		chosenFighter.equippedArmor = fighters[fighterIndex_].equippedArmor;
		chosenFighter.equippedAccessory = fighters[fighterIndex_].equippedAccessory;
//		chosenFighter.emitterIndex = fighters[fighterIndex_].emitterIndex;

		currentIndex = fighterIndex_;
		haveChosenFighter = true;
	}
	
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


FString AFightersShop::PrintFighter(int index_)
{
	// load all of the stat values and print them to a string
	if (haveChosenFighter && currentIndex == index_)
	{
		return FString("HP: " + FString::FromInt(chosenFighter.hp) +
			" PIP: " + FString::FromInt(chosenFighter.pip) +
			" ATK: " + FString::FromInt(chosenFighter.atk) +
			" DEF: " + FString::FromInt(chosenFighter.def) +
			" INT: " + FString::FromInt(chosenFighter.intl) +
			" SPD: " + FString::FromInt(chosenFighter.spd) +
			" CRIT: " + FString::FromInt(chosenFighter.crit) +
			" AGL: " + FString::FromInt(chosenFighter.agl) +
			" CRD: " + FString::FromInt(chosenFighter.crd));
	}
	else
	{
		FFighterTableStruct row = fileReader->FindFighterTableRow(rowNames[index_], 0);
		return FString("HP: " + FString::FromInt(row.hp) + " PIP: " + FString::FromInt(row.pip) + " ATK: " + FString::FromInt(row.atk) + " DEF: " + FString::FromInt(row.def) + " INT: " + FString::FromInt(row.intl) + " SPD: " + FString::FromInt(row.spd) + " CRIT: " + FString::FromInt(row.crit) + " AGL: " + FString::FromInt(row.agl) + " CRD: " + FString::FromInt(row.crd));
	}
}

void AFightersShop::LevelUpFighter()
{
	if (haveChosenFighter)
		chosenFighter.LevelUpUntilGoal(chosenFighter.level + 1);
}

void AFightersShop::LevelDownFighter()
{
	if (haveChosenFighter)
		chosenFighter.LevelUpUntilGoal(chosenFighter.level - 1);
}

void AFightersShop::FinalizePurchase()
{
	
	// load all of the chosen fighters values into the data table;

	FFighterTableStruct price = fileReader->FindFighterTableRow(rowNames[currentIndex], 0);
	if (haveChosenFighter)
	{
		CalculatePrice();
		if (Intermediate::GetInstance()->GetCurrentMoney() > chosenFighter.value)
		{
			Intermediate::GetInstance()->SpendMoney(chosenFighter.value);

			FFighterTableStruct row;
			row.bpid = chosenFighter.bpid;
			row.name = chosenFighter.name;
			row.level = chosenFighter.level;
			row.currentEXP = chosenFighter.currentEXP;
			row.neededEXPToLevelUp = chosenFighter.neededEXPToLevelUp;
			row.weaponIndex = chosenFighter.weaponIndex;
			row.armorIndex = chosenFighter.armorIndex;
			row.equippedWeapon = chosenFighter.equippedWeapon;
			row.equippedArmor = chosenFighter.equippedArmor;
			row.equippedAccessory = chosenFighter.equippedAccessory;
//			row.emitterIndex = chosenFighter.emitterIndex;
			row.hp = chosenFighter.hp;
			row.pip = chosenFighter.pip;
			row.atk = chosenFighter.atk;
			row.def = chosenFighter.def;
			row.intl = chosenFighter.intl;
			row.spd = chosenFighter.spd;
			row.crit = chosenFighter.crit;
			row.agl = chosenFighter.agl;
			row.crd = chosenFighter.crd;
			row.archetype = chosenFighter.archetype;
			row.id = ++fighterID; //Add 1 to fighter ID then make that equal to row id. Makes sure no two fighters have the same ID
			row.value = 0;
			// add the new row to the table
			FName index = FName(*FString::FromInt(purchasedFighters));
			fileReader->AddRowToRecruitedFighterTable(index, 1, row);
			purchasedFighters += 1;
			// adds one to the roster size
			Intermediate::GetInstance()->UpdateCurrentRosterSize(1);
			//TODO
			//Play a UI pop up
			UE_LOG(LogTemp, Warning, TEXT("Finalize Purchase"));
		}
		else
		{
			//TODO
			//Play UI pop up
			UE_LOG(LogTemp, Error, TEXT("Not enough money"));
		}
	}
	
}

void AFightersShop::CalculatePrice()
{
	chosenFighter.CalculatePrice();
}

void AFightersShop::LoadText()
{
	// empty function because we don't want the values on a dialogue table to be loaded
}
