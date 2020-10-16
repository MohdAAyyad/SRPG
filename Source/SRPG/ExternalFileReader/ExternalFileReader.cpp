// Fill out your copyright notice in the Description page of Project Settings.


#include "ExternalFileReader.h"
#include "Definitions.h"


// Sets default values for this component's properties
UExternalFileReader::UExternalFileReader()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UExternalFileReader::BeginPlay()
{
	Super::BeginPlay();
	// arbitrary value that will be changed to the amount of tables we have
	tables.Reserve(10);
	// ...
	
}

FDialogueTableStruct UExternalFileReader::FindDialogueTableRow(FName name_, int index_)
{
	static const FString contextString(TEXT("Dialogue Table"));
	if (tables[index_])
	{
		FDialogueTableStruct* result = tables[index_]->FindRow<FDialogueTableStruct>(name_, contextString, true);
		return *result;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Dialogue Table returned NULL"));
		return FDialogueTableStruct();
	}
}

FFighterTableStruct UExternalFileReader::FindFighterTableRow(FName name_, int index_)
{
	static const FString contextString(TEXT("Fighter Table"));
	// go clear the recruited fighters table if it's the first time running 

	if (tables[index_])
	{
		FFighterTableStruct* result = tables[index_]->FindRow<FFighterTableStruct>(name_, contextString, true);
		return *result;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Fighter Table returned NULL"));
		return FFighterTableStruct();
	}
}

FItemTableStruct UExternalFileReader::FindItemTableRow(FName name_, int index_)
{
	static const FString contextString(TEXT("Item Table"));
	
	if (tables[index_])
	{
		// otherwise return the result
		FItemTableStruct* result = tables[index_]->FindRow<FItemTableStruct>(name_, contextString, true);
		return *result;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Item Table returned NULL"));
		return FItemTableStruct();
	}
}

FEquipmentTableStruct UExternalFileReader::FindEquipmentTableRow(FName name_, int index_)
{
	static const FString contextString(TEXT("Equipment Table"));
	if (tables[index_])
	{
		FEquipmentTableStruct* result = tables[index_]->FindRow<FEquipmentTableStruct>(name_, contextString, true);
		return *result;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Equipment Table returned NULL"));
		return FEquipmentTableStruct();
	}
}

FFighterTableStruct UExternalFileReader::FindEquippedFighterTableRow(FName name_, int index_)
{
	static const FString contextString(TEXT("Equipped Fighters Table"));

	if (tables[index_])
	{
		FFighterTableStruct* result = tables[index_]->FindRow<FFighterTableStruct>(name_, contextString, true);
		return *result;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Equipped Fighters Table returned NULL"));
		return FFighterTableStruct();
	}
}

FActivityDialogueTableStruct UExternalFileReader::FindActivityDialogueTableRow(FName name_, int index_)
{
	static const FString contextString(TEXT("Activity Dialogue Table"));
	if (tables[index_])
	{
		FActivityDialogueTableStruct* result = tables[index_]->FindRow<FActivityDialogueTableStruct>(name_, contextString, true);
		return *result;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Activity Dialogue Table returned NULL"));
		return FActivityDialogueTableStruct();
	}
}

FActivityDialogueTableStruct UExternalFileReader::GetPositiveBranch(int activityIndex_, int tableIndex_)
{
	if (tables[tableIndex_])
	{
		TArray<FName> rowNames;
		rowNames = tables[tableIndex_]->GetRowNames();
		// loop through all the rows and find the row that matches the activity index, is a branch, and is positive
		for (auto n : rowNames)
		{
			FActivityDialogueTableStruct result = FindActivityDialogueTableRow(n, tableIndex_);

			if (result.activityIndex == activityIndex_ && result.branchOrCentral == 0 
				&& result.positiveOrNegative == 1 && result.startingDialogue == 0 && result.endDialogue == 0)
			{
				// return it
				return result;
			}
		}
		UE_LOG(LogTemp, Error, TEXT("No Positive Branch Row Found"));
		// if nothing is found return an empty struct
		return FActivityDialogueTableStruct();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Activity Table returned NULL"));
		// if NULL return empty
		return FActivityDialogueTableStruct();
	}
}

FActivityDialogueTableStruct UExternalFileReader::GetNegativeBranch(int activityIndex_, int tableIndex_)
{
	if (tables[tableIndex_])
	{
		TArray<FName> rowNames;
		rowNames = tables[tableIndex_]->GetRowNames();
		// loop through all the rows and find the row that matches the activity index, is a branch, and is negative
		for (auto n : rowNames)
		{
			FActivityDialogueTableStruct result = FindActivityDialogueTableRow(n, tableIndex_);

			if (result.activityIndex == activityIndex_ && result.branchOrCentral == 0 
				&& result.positiveOrNegative == 0 && result.startingDialogue == 0 && result.endDialogue == 0)
			{
				// return it
				return result;
			}
		}
		UE_LOG(LogTemp, Error, TEXT("No Negative Branch Row Found"));
		// if nothing is found return an empty struct
		return FActivityDialogueTableStruct();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Activity Table returned NULL"));
		// if NULL return empty
		return FActivityDialogueTableStruct();
	}
}

FActivityDialogueTableStruct UExternalFileReader::GetPositiveCentral(int activityIndex_, int tableIndex_)
{
	if (tables[tableIndex_])
	{
		TArray<FName> rowNames;
		rowNames = tables[tableIndex_]->GetRowNames();
		// loop through all the rows and find the row that matches the activity index, is a central, and is positive
		for (auto n : rowNames)
		{
			FActivityDialogueTableStruct result = FindActivityDialogueTableRow(n, tableIndex_);

			if (result.activityIndex == activityIndex_ && result.branchOrCentral == 1 
				&& result.positiveOrNegative == 1 && result.startingDialogue == 0 && result.endDialogue == 0)
			{
				// return it
				return result;
			}
		}
		UE_LOG(LogTemp, Error, TEXT("No Positive Central Row Found"));
		// if nothing is found return an empty struct
		return FActivityDialogueTableStruct();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Activity Table returned NULL"));
		// if NULL return empty
		return FActivityDialogueTableStruct();
	}
}

FActivityDialogueTableStruct UExternalFileReader::GetNegativeCentral(int activityIndex_, int tableIndex_)
{
	if (tables[tableIndex_])
	{
		TArray<FName> rowNames;
		rowNames = tables[tableIndex_]->GetRowNames();
		// loop through all the rows and find the row that matches the activity index, is a central, and is positive
		for (auto n : rowNames)
		{
			FActivityDialogueTableStruct result = FindActivityDialogueTableRow(n, tableIndex_);

			if (result.activityIndex == activityIndex_ && result.branchOrCentral == 1 
				&& result.positiveOrNegative == 0 && result.startingDialogue == 0 && result.endDialogue == 0)
			{
				// return it
				return result;
			}
		}
		UE_LOG(LogTemp, Error, TEXT("No Negative Central Row Found"));
		// if nothing is found return an empty struct
		return FActivityDialogueTableStruct();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Activity Table returned NULL"));
		// if NULL return empty
		return FActivityDialogueTableStruct();
	}
}

FActivityDialogueTableStruct UExternalFileReader::GetStartingDialogue(int activityIndex_, int tableIndex_)
{
	if (tables[tableIndex_])
	{
		TArray<FName> rowNames;
		rowNames = tables[tableIndex_]->GetRowNames();
		for (FName n : rowNames)
		{
			FActivityDialogueTableStruct result = FindActivityDialogueTableRow(n, tableIndex_);

			if (result.activityIndex == activityIndex_ && result.startingDialogue == 1 && result.endDialogue == 0)
			{
				return result;
			}
		}
		UE_LOG(LogTemp, Error, TEXT("No Starting Dialogue Row Found"));
		return FActivityDialogueTableStruct();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Activity Table returned NULL"));
		return FActivityDialogueTableStruct();
	}
}

FActivityDialogueTableStruct UExternalFileReader::GetActivityEndDialogue(int activityIndex_, int tableIndex_)
{
	if (tables[tableIndex_])
	{
		TArray<FName> rowNames;
		rowNames = tables[tableIndex_]->GetRowNames();
		for (FName n : rowNames)
		{
			FActivityDialogueTableStruct result = FindActivityDialogueTableRow(n, tableIndex_);

			if (result.activityIndex == activityIndex_ && result.startingDialogue == 0 && result.endDialogue == 1)
			{
				return result;
			}
		}
		UE_LOG(LogTemp, Error, TEXT("No Ending Dialogue Row Found"));
		return FActivityDialogueTableStruct();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Activity Table returned NULL"));
		return FActivityDialogueTableStruct();
	}
}

void UExternalFileReader::AddRowToRecruitedFighterTable(FName rowName_, int index_, FFighterTableStruct row_)
{
	if (tables[index_])
	{
		tables[index_]->AddRow(rowName_, row_);
	}
}

TArray<FFighterTableStruct> UExternalFileReader::GetAllRecruitedFighters(int tableIndex_)
{

	// Didn't want to break your old version so I made a new one that actually checks for the table index instead of a hardcoded
	//value
	static const FString contextString(TEXT("Trying to get the recruited fighters from the table"));
	TArray<FName> rowNames;
	TArray<FFighterTableStruct> rfighters;
	if (tables[tableIndex_])
	{
		rowNames = tables[tableIndex_]->GetRowNames(); //Will only be accessed by TransitionToBattle in the battle. 0 for Rfighters.

		for (auto n : rowNames)
		{
			rfighters.Push(*tables[tableIndex_]->FindRow<FFighterTableStruct>(n, contextString, true));
		}

		return rfighters;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Fighter Table returned NULL"));
		return TArray<FFighterTableStruct>();
	}

	
}

FFighterTableStruct UExternalFileReader::GetRecruitedFighterByID(int id_)
{
	static const FString contextString(TEXT("Trying to get the items from the table"));
	TArray<FName> rowNames;
	rowNames = tables[0]->GetRowNames(); //Called by battle manager to update the stats of the fighters

	for (auto n : rowNames)
	{
		FFighterTableStruct* row = tables[0]->FindRow<FFighterTableStruct>(n, contextString, true);
		if (row->id  == id_)
		{
			return *row;
		}
	}

	return FFighterTableStruct();

}

//Called by the fighter shop on being play if the ids_ array is larger than zero in size
void UExternalFileReader::RemoveFightersDueToPermaDeath(TArray<int>& ids_, int tableIndex_)
{
	static const FString contextString(TEXT("Trying to remove fighters from the recruited fighters table"));
	TArray<FName> rowNames;
	rowNames = tables[tableIndex_]->GetRowNames(); //Get all the rows

	for (int i = rowNames.Num()-1; i>-1; i--)
	{
		FFighterTableStruct* row = tables[tableIndex_]->FindRow<FFighterTableStruct>(rowNames[i], contextString, true); //Get the fighter struct row
		if (ids_.Contains(row->id)) //Is the fighter in the table part of the array of dead units?
		{
			tables[tableIndex_]->RemoveRow(rowNames[i]);
		}
	}
}

//Called by the fighter shop on being play if the selected fighter's array is larger than zero in size
void UExternalFileReader::IncreaseTheStatsOfThisFigheter(FFighterTableStruct fighter_, int tableIndex_)
{
	static const FString contextString(TEXT("Trying to increase the stats of the fighters in the recruited fighters table"));
	TArray<FName> rowNames;
	rowNames = tables[tableIndex_]->GetRowNames(); //Get all the rows

	for (int i = rowNames.Num() - 1; i > -1; i--)
	{
		FFighterTableStruct* row = tables[tableIndex_]->FindRow<FFighterTableStruct>(rowNames[i], contextString, true); //Get the fighter struct row
		if (fighter_.id == row->id) //Is the fighter in the table part of the array of dead units?
		{
			row->hp = fighter_.hp;
			row->pip = fighter_.pip;
			row->atk = fighter_.atk;
			row->def = fighter_.def;
			row->intl = fighter_.intl;
			row->spd = fighter_.spd;
			row->agl = fighter_.agl;
			row->crit = fighter_.crit;
			row->crd = fighter_.crd;
			row->currentEXP = fighter_.currentEXP;
			row->neededEXPToLevelUp = fighter_.neededEXPToLevelUp;
			row->level = fighter_.level;
		}
	}

}

void UExternalFileReader::AddOwnedValueItemTable(FName rowName_, int index_, int value_)
{
	static const FString contextString(TEXT("Item Table"));

	if (tables[index_])
	{
		FItemTableStruct* row = tables[index_]->FindRow<FItemTableStruct>(rowName_, contextString, true);
		if (row)
		{
			UE_LOG(LogTemp, Error, TEXT("Found Row"));
			row->owned += value_;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Item Table returned NULL"));
	}
}

void UExternalFileReader::AddOwnedValueEquipmentTable(FName rowName_, int index_, int value_)
{
	static const FString contextString(TEXT("Equipment Table"));

	if (tables[index_])
	{
		FEquipmentTableStruct* row = tables[index_]->FindRow<FEquipmentTableStruct>(rowName_, contextString, true);
		row->owned += value_;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Item Table returned NULL"));
	}
}


UExternalFileReader* UExternalFileReader::GetExternalFileReader()
{
	return this;
}

UDataTable* UExternalFileReader::GetTable(int index_)
{
	return tables[index_];
}

//NOTE:
/* The name Input for the struct name must match the struct name. It is not case sensative. When you pass in the name make you 
pass in the name of the file. EX: You would reference the item table struct as FItemTableStruct in code normally, but to pass
it in here you would pass it in as ItemTableStruct since F is not part of the name. In short pass in the struct file name. */

int UExternalFileReader::FindTableIndexInArray(FName structName_)
{
	int result = 0;
	for (auto t : tables)
	{
		FName structQuery = t->GetRowStructName();
		if (structQuery == structName_)
		{
			return result;
		}
		result++;
	}
	UE_LOG(LogTemp, Error, TEXT("Could not find valid table or invalid name input"));
	return result;
}

// same story with the name one. Input the name correctly and you have nothing to worry about
int UExternalFileReader::FindTableIndexByName(FName tableName_)
{
	int result = 0;
	for (auto t : tables)
	{
		FName tableQuery = t->GetFName();
		if (tableQuery == tableName_)
		{
			return result;
		}
		result++;
	}

	UE_LOG(LogTemp, Error, TEXT("Could not find valid table or invalid name input"));
	return result;
}

TArray<FSkillTableStruct*> UExternalFileReader::GetOffesniveSkills(int tableIndex_, int weaponIndex_, int skillNum_, int skillsIndex_, int currentLevel_)
{
	static const FString contextString(TEXT("Trying to get the skills from the table"));
	TArray<FName> rowNames;
	TArray<FSkillTableStruct*> skills;
	int skillsAddedSoFar = 0;
	rowNames = tables[tableIndex_]->GetRowNames(); //Will only be accessed by fighters in the battle. 0 for skills.

	for (auto n : rowNames)
	{
		if (skillsAddedSoFar < skillNum_) //Each weapon gives a set number of skills
		{
			FSkillTableStruct* row = tables[tableIndex_]->FindRow<FSkillTableStruct>(n, contextString, true);

			//If we have the right weapon type, the correct skill index and the correct level, then this is valid information
			if (row->weaponIndex == weaponIndex_ && row->equipmentSkillsIndex == skillsIndex_ && row->levelToUnlock <= currentLevel_)
			{
				//UE_LOG(LogTemp,Warning,TEXT("Pushed into skills"));
				skills.Push(row);
				skillsAddedSoFar++;
			}
		}
		else //We've got enough skills, break
		{
			break;
		}
	}

	return skills;
}

//TODO
//Update the below function and the skills struct so that you can use armor skills
TArray<FSkillTableStruct*> UExternalFileReader::GetDefensiveSkills(int tableIndex_,int armorIndex_, int skillNum_, int skillsIndex_, int currentLevel_)
{
	static const FString contextString(TEXT("Trying to get the skills from the table"));
	TArray<FName> rowNames;
	TArray<FSkillTableStruct*> skills;
	int skillsAddedSoFar = 0;
	rowNames = tables[tableIndex_]->GetRowNames(); //Will only be accessed by fighters in the battle. 0 for skills.

	for (auto n : rowNames)
	{
		if (skillsAddedSoFar < skillNum_) //Each weapon gives a set number of skills
		{
			FSkillTableStruct* row = tables[tableIndex_]->FindRow<FSkillTableStruct>(n, contextString, true);

			//If we have the right armor type, the correct skill index and the correct level, then this is valid information
			if ( row->weaponIndex == armorIndex_ && row->equipmentSkillsIndex == skillsIndex_ && row->levelToUnlock <= currentLevel_)
			{
				//UE_LOG(LogTemp,Warning,TEXT("Pushed into skills"));
				skills.Push(row);
				skillsAddedSoFar++;
			}
		}
		else //We've got enough skills, break
		{
			break;
		}
	}

	return skills;
}

TArray<FItemTableStruct> UExternalFileReader::GetAllOwnedItems()
{
	static const FString contextString(TEXT("Trying to get the items from the table"));
	TArray<FName> rowNames;
	TArray<FItemTableStruct> items;
	rowNames = tables[3]->GetRowNames(); //Will only be accessed by fighters in the battle. 1 for items.

	for (auto n : rowNames)
	{
		FItemTableStruct* row = tables[3]->FindRow<FItemTableStruct>(n, contextString, true);
		if (row->owned > 0)
		{
			items.Push(*row);
		}
	}

	return items;
}

int UExternalFileReader::GetItemStatIndex(int tableIndex_, FName itemName_)
{
	static const FString contextString(TEXT("Trying to get item's stat index from table"));
	FItemTableStruct* row = tables[tableIndex_]->FindRow<FItemTableStruct>(itemName_, contextString, true);

	if(row)
		return row->statIndex;

	return 0;
}
int UExternalFileReader::GetItemValue(FName itemName_)
{
	static const FString contextString(TEXT("Trying to get item's value from table"));
	FItemTableStruct* row = tables[3]->FindRow<FItemTableStruct>(itemName_, contextString, true);

	if (row)
		return row->value;

	return 0;
}


void UExternalFileReader::RemoveFighterTableRow(FName rowName_, int tableIndex_)
{
	if (tables[tableIndex_])
	{
		UE_LOG(LogTemp, Warning, TEXT("Fighter row  removed"));
		tables[tableIndex_]->RemoveRow(rowName_);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Fighter Table returned NULL"));
	}

}

FName UExternalFileReader::ConvertItemNameToNameUsedInTable(FName name_)
{
	FString n = name_.ToString();

	if (n == "Healing Potion")
		return FName(TEXT("0"));

	if (n == "PIP restore")
		return FName(TEXT("1"));

	return FName(TEXT(""));
}

FEquipmentTableStruct UExternalFileReader::GetEquipmentById(int tableIndex_,int equipID_, int equipIndex_, int subIndex_)
{
	static const FString contextString(TEXT("Trying to get the Weapons from the table based on ID"));
	TArray<FName> rowNames;
	FEquipmentTableStruct equip;
	if (tableIndex_ >= 0 && tableIndex_ < tables.Num())
	{
		rowNames = tables[tableIndex_]->GetRowNames(); //Will only be accessed by fighters in the battle.

		// UE_LOG(LogTemp, Warning, TEXT("Incoming equipIndex_ %d"), equipIndex_);

		for (auto n : rowNames)
		{
			FEquipmentTableStruct* row = tables[tableIndex_]->FindRow<FEquipmentTableStruct>(n, contextString, true);

		//	UE_LOG(LogTemp, Warning, TEXT("Row equipmentIndex %d"), row->equipmentIndex);
			if (row->equipmentIndex == equipIndex_) //Is this the correct piece of equipment
			{
				switch (equipIndex_) //Find out what type of equipment this is
				{
				case EQU_WPN:
					if(row->weaponIndex == subIndex_) //Is this the correct type of weapon?
						if (row->id == equipID_) //Is this the correct ID within the weapon/weaponclass tree?
						{
							equip = *row;
							return equip;

						}
					break;
				case EQU_ARM:
					if (row->armorIndex == subIndex_) //Is this the correct type of armor?
						if (row->id == equipID_) //Is this the correct ID within the weapon/weaponclass tree?
						{
							equip = *row;
							return equip;

						}
					break;
				case EQU_ACC: //Accessories don't have sub indexes
					if (row->id == equipID_) //Is this the correct ID within the weapon/weaponclass tree?
					{
						equip = *row;
						return equip;
					}
					break;
				}

			}

		}
	}

	return equip;
}

FEquipmentTableStruct UExternalFileReader::GetEquipmentByLevel(int tableIndex_, int level_, int equipIndex_, int subIndex_)
{
	static const FString contextString(TEXT("Trying to get the Weapons from the table based on level"));
	TArray<FName> rowNames;
	FEquipmentTableStruct equip;
	int maxLevel = -1;
	if (tableIndex_ >= 0 && tableIndex_ < tables.Num())
	{
		rowNames = tables[tableIndex_]->GetRowNames(); //Will only be accessed by enemies in the battle.

		for (auto n : rowNames)
		{
			//Find the max level equipment that matches your equipment index
			FEquipmentTableStruct* row = tables[tableIndex_]->FindRow<FEquipmentTableStruct>(n, contextString, true);
			//UE_LOG(LogTemp, Warning, TEXT("Row details L: %d E: %d W: %d"), row->level, row->equipmentIndex, row->weaponIndex);
			//UE_LOG(LogTemp, Warning, TEXT("Incoming details L: %d E: %d W: %d"), level_, equipIndex_, subIndex_);
			//Check what type of equipment it is
			// Then return the highest level equip-able piece of equipment of that type
			if (row->equipmentIndex == equipIndex_)
			{
				switch (equipIndex_)
				{
				case EQU_WPN:
					if (row->weaponIndex == subIndex_)
					{
						if (row->level <= level_ && row->level > maxLevel)
						{

							equip = *row;
							maxLevel = row->level;

						}
					}
					break;
				case EQU_ARM:
					if (row->armorIndex == subIndex_)
					{
						if (row->level <= level_ && row->level > maxLevel)
						{
							equip = *row;
							maxLevel = row->level;

						}
					}
					break;
				case EQU_ACC:
					if (row->level <= level_ && row->level > maxLevel)
					{
						equip = *row;
						maxLevel = row->level;

					}
					break;
				}
			}

		}
	}

	return equip;
}

TArray<FItemTableStruct> UExternalFileReader::GetAllItems(int tableIndex_, int worldLevel_)
{
	static const FString contextString(TEXT("Getting all items"));
	TArray<FItemTableStruct> results;
	TArray<FName> rowNames;
	if (tables[tableIndex_])
	{
		rowNames = tables[tableIndex_]->GetRowNames();

		for (auto n : rowNames)
		{
			FItemTableStruct* row = tables[tableIndex_]->FindRow<FItemTableStruct>(n, contextString, true);
			if (row->level <= worldLevel_)
			{
				results.Push(*row);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Item Table returned NULL"));
	}

	return results;
}

TArray<FEquipmentTableStruct> UExternalFileReader::GetAllEquipment(int tableIndex_, int worldLevel_)
{
	static const FString contextString(TEXT("Getting all equipment"));
	TArray<FEquipmentTableStruct> results;
	TArray<FName> rowNames;
	if (tables[tableIndex_])
	{
		rowNames = tables[tableIndex_]->GetRowNames();
		for (auto n : rowNames)
		{
			FEquipmentTableStruct* row = tables[tableIndex_]->FindRow<FEquipmentTableStruct>(n, contextString, true);
			if (row->level <= worldLevel_)
			{
				results.Push(*row);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Equipment Table returned NULL"));
	}

	return results;
}
