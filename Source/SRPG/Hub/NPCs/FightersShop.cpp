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
	PrimaryActorTick.bCanEverTick = false;
}
void AFightersShop::BeginPlay()
{
	Super::BeginPlay();
	fighterID = Intermediate::GetInstance()->GetLatestFighterID();
	GetAllFightersForSale();

	if (Intermediate::GetInstance()->GetDeadUnits().Num() > 0)
	{
		if (fileReader)
		{
			fileReader->RemoveFightersDueToPermaDeath(Intermediate::GetInstance()->GetDeadUnits(), 1); //Remove any dead units from the recruited fighters table
		}
	}
	warning = "";

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
			FFighterTableStruct row = fileReader->FindFighterTableRow(n, fileReader->FindTableIndexByName("AllFighters__DataTable1SS"));
			//add the fighter to the array
			fighters.Push(row);
		}
		return fighters;
	}
	warning = "";
	return TArray<FFighterTableStruct>();
}

TArray<UTexture*> AFightersShop::GetTextureArray()
{
	return textures;
}

void AFightersShop::SetWarningText(FString text_)
{
	warning = text_;
}

void AFightersShop::SetHasSelectedFighter(bool hasSelectedFighter_)
{
	haveChosenFighter = hasSelectedFighter_;
}

bool AFightersShop::IsAffordable()
{
	if (haveChosenFighter)
	{
		if (Intermediate::GetInstance()->GetCurrentMoney() - chosenFighter.value > 0)
		{
			return true;
		}
		else
		{
			return false;
			warning = "Not Enough Money";
		}
	}
	else
	{
		warning = "No Fighter Selected";
		return false;
	}

}

FString AFightersShop::GetWarning()
{
	return warning;
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
		warning = "";
	}
	
}

void AFightersShop::UpdateName(FString name_)
{
	if (haveChosenFighter)
	{
		chosenFighter.name = name_;
		
	}
	else
	{
		warning = "No fighter selected";
	}

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

FFighterTableStruct AFightersShop::LevelUpFighterStruct()
{
	FFighterTableStruct result;
	if (haveChosenFighter)
	{
		chosenFighter.LevelUpUntilGoal(chosenFighter.level + 1);
		
		result.hp = chosenFighter.hp;
		result.pip = chosenFighter.pip;
		result.atk = chosenFighter.atk;
		result.def = chosenFighter.def;
		result.intl = chosenFighter.intl;
		result.spd = chosenFighter.spd;
		result.crit = chosenFighter.crit;
		result.agl = chosenFighter.agl;
		result.crd = chosenFighter.crd;
		result.value = chosenFighter.value;

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Not chosen Fighter"));
		warning = "No fighter selected";
	}
	return result;
	
}

FFighterTableStruct AFightersShop::LevelDownFighterStruct()
{
	FFighterTableStruct result;
	if (haveChosenFighter)
	{
		chosenFighter.LevelUpUntilGoal(chosenFighter.level - 1);

		result.hp = chosenFighter.hp;
		result.pip = chosenFighter.pip;
		result.atk = chosenFighter.atk;
		result.def = chosenFighter.def;
		result.intl = chosenFighter.intl;
		result.spd = chosenFighter.spd;
		result.crit = chosenFighter.crit;
		result.agl = chosenFighter.agl;
		result.crd = chosenFighter.crd;
		result.value = chosenFighter.value;

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Not chosen Fighter"));
		warning = "No fighter selected";
	}
	return result;
}

void AFightersShop::LevelDownFighter()
{
	if (haveChosenFighter)
		chosenFighter.LevelUpUntilGoal(chosenFighter.level - 1);
}

void AFightersShop::FinalizePurchase()
{

	// load all of the chosen fighters values into the data table;

	//FFighterTableStruct price = fileReader->FindFighterTableRow(rowNames[currentIndex], 0);
	if (haveChosenFighter == false)
	{
		warning = "No Fighter Selected";

	}
	else
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
			fileReader->AddRowToRecruitedFighterTable(index, fileReader->FindTableIndexByName("RecruitedFighters_DataTable1_"), row);
			purchasedFighters += 1;
			// adds one to the roster size
			Intermediate::GetInstance()->UpdateCurrentRosterSize(1);
			//TODO
			//Play a UI pop up
			UE_LOG(LogTemp, Warning, TEXT("Finalize Purchase"));
			haveChosenFighter = false;
			warning = "";
		}
		else
		{
			//TODO
			//Play UI pop up
			UE_LOG(LogTemp, Error, TEXT("Not enough money"));
			warning = "Not enough money to purchase fighter";
		}
	}
}
	

void AFightersShop::CalculatePrice()
{
	if (haveChosenFighter)
	{
		chosenFighter.CalculatePrice(chosenFighter.level);
	}
	else
	{
		warning = "No Fighter Selected";
	}

}

void AFightersShop::LoadText()
{
	// empty function because we don't want the values on a dialogue table to be loaded
}
