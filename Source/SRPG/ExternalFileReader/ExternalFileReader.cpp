// Fill out your copyright notice in the Description page of Project Settings.


#include "ExternalFileReader.h"



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
	firstTimeFighter = true;
	firstTimeItem = true;
	firstTimeEquipment = true;
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
		//reset our owned values to 0 in the table
		if (firstTimeItem)
		{
			TArray<FName> rowNames;
			rowNames = tables[index_]->GetRowNames();
			for (auto n : rowNames)
			{
				FItemTableStruct* row = tables[index_]->FindRow<FItemTableStruct>(n, contextString, true);
				row->owned = 0;
			}

			firstTimeItem = false;
		}
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
		if (firstTimeEquipment)
		{
			TArray<FName> rowNames;
			rowNames = tables[index_]->GetRowNames();
			for (auto n : rowNames)
			{
				FEquipmentTableStruct* row = tables[index_]->FindRow<FEquipmentTableStruct>(n, contextString, true);
				row->owned = 0;
			}
			firstTimeEquipment = false;
		}

		FEquipmentTableStruct* result = tables[index_]->FindRow<FEquipmentTableStruct>(name_, contextString, true);
		return *result;

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Equipment Table returned NULL"));
		return FEquipmentTableStruct();
	}
}

FEquippedFightersTableStruct UExternalFileReader::FindEquippedFighterTableRow(FName name_, int index_)
{
	static const FString contextString(TEXT("Equipped Fighters Table"));
	if (tables[index_])
	{
		FEquippedFightersTableStruct* result = tables[index_]->FindRow<FEquippedFightersTableStruct>(name_, contextString, true);
		return *result;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Equipped Fighters Table returned NULL"));
		return FEquippedFightersTableStruct();
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

			if (result.activityIndex == activityIndex_ && result.branchOrCentral == 0 && result.positiveOrNegative == 1)
			{
				// return it
				return result;
			}
		}
		UE_LOG(LogTemp, Error, TEXT("No Row Found"));
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

			if (result.activityIndex == activityIndex_ && result.branchOrCentral == 0 && result.positiveOrNegative == 0)
			{
				// return it
				return result;
			}
		}
		UE_LOG(LogTemp, Error, TEXT("No Row Found"));
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

			if (result.activityIndex == activityIndex_ && result.branchOrCentral == 1 && result.positiveOrNegative == 1)
			{
				// return it
				return result;
			}
		}
		UE_LOG(LogTemp, Error, TEXT("No Row Found"));
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

			if (result.activityIndex == activityIndex_ && result.branchOrCentral == 1 && result.positiveOrNegative == 0)
			{
				// return it
				return result;
			}
		}
		UE_LOG(LogTemp, Error, TEXT("No Row Found"));
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

void UExternalFileReader::AddRowToFighterTable(FName rowName_, int index_, FEquippedFightersTableStruct row_)
{
	
	if (tables[index_])
	{
		if (firstTimeFighter)
		{
			TArray<FName> rowNames;
			rowNames = tables[index_]->GetRowNames();
			for (auto n : rowNames)
			{
				tables[index_]->RemoveRow(n);
			}
			UE_LOG(LogTemp, Warning, TEXT("Deleted Equipped Fighter Table"));
			firstTimeFighter = false;
		}
		tables[index_]->AddRow(rowName_, row_);
		UE_LOG(LogTemp, Error, TEXT("Fighter Table Row Added!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Fighter Table returned NULL"));
	}
}

TArray<FFighterTableStruct*> UExternalFileReader::GetAllRecruitedFighters()
{
	//Called by the TransitionToBattle actor

	static const FString contextString(TEXT("Trying to get the recruited fighters from the table"));
	TArray<FName> rowNames;
	TArray<FFighterTableStruct*> rfighters;
	rowNames = tables[0]->GetRowNames(); //Will only be accessed by TransitionToBattle in the battle. 0 for Rfighters.

	for (auto n : rowNames)
	{
		rfighters.Push(tables[0]->FindRow<FFighterTableStruct>(n, contextString, true));
	}

	return rfighters;
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
	//reset our owned values to 0 in the table
	if (firstTimeEquipment)
	{
		TArray<FName> rowNames;
		rowNames = tables[index_]->GetRowNames();
		for (auto n : rowNames)
		{
			FEquipmentTableStruct* row = tables[index_]->FindRow<FEquipmentTableStruct>(n, contextString, true);
			row->owned = 0;
		}

		firstTimeEquipment = false;
	}

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


TArray<FSkillTableStruct*> UExternalFileReader::GetSkills(int weaponIndex_, int currentLevel_)
{
	static const FString contextString(TEXT("Trying to get the skills from the table"));
	TArray<FName> rowNames;
	TArray<FSkillTableStruct*> skills;
	rowNames = tables[0]->GetRowNames(); //Will only be accessed by fighters in the battle. 0 for skills.

	for (auto n : rowNames)
	{
		FSkillTableStruct* row = tables[0]->FindRow<FSkillTableStruct>(n, contextString, true);
		
		//If we have the right weapon and the correct level, then this is valid information
		if (row->weaponIndex == weaponIndex_ && row->levelToUnlock <= currentLevel_) 
		{
			//UE_LOG(LogTemp,Warning,TEXT("Pushed into skills"));
			skills.Push(row);
		}
	}

	return skills;
}


TArray<FItemTableStruct> UExternalFileReader::GetAllOwnedItems()
{
	static const FString contextString(TEXT("Trying to get the items from the table"));
	TArray<FName> rowNames;
	TArray<FItemTableStruct> items;
	rowNames = tables[1]->GetRowNames(); //Will only be accessed by fighters in the battle. 1 for items.

	for (auto n : rowNames)
	{
		FItemTableStruct* row = tables[1]->FindRow<FItemTableStruct>(n, contextString, true);
		if (row->owned > 0)
		{
			items.Push(*row);
		}
	}

	return items;
}

int UExternalFileReader::GetItemStatIndex(FName itemName_)
{
	static const FString contextString(TEXT("Trying to get item's stat index from table"));
	FItemTableStruct* row = tables[1]->FindRow<FItemTableStruct>(itemName_, contextString, true);

	if(row)
		return row->statIndex;

	return 0;
}
int UExternalFileReader::GetItemValue(FName itemName_)
{
	static const FString contextString(TEXT("Trying to get item's value from table"));
	FItemTableStruct* row = tables[1]->FindRow<FItemTableStruct>(itemName_, contextString, true);

	if (row)
		return row->value;

	return 0;
}


FName UExternalFileReader::ConvertItemNameToNameUsedInTable(FName name_)
{
	FString n = name_.ToString();

	if (n == "Healing Potion")
		return FName(TEXT("1"));

	if (n == "PIP restore")
		return FName(TEXT("2"));

	return FName(TEXT(""));
}