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
				FItemTableStruct* row = tables[index_]->FindRow<FItemTableStruct>(name_, contextString, true);
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

void UExternalFileReader::AddRowToFighterTable(FName rowName_, int index_, FFighterTableStruct row_)
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

void UExternalFileReader::AddOwnedValueItemTable(FName rowName_, int index_, int value_)
{
	static const FString contextString(TEXT("Item Table"));
	if (tables[index_])
	{
		FItemTableStruct* row = tables[index_]->FindRow<FItemTableStruct>(rowName_, contextString, true);
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

UDataTable * UExternalFileReader::GetTable(int index_)
{
	return tables[index_];
}

