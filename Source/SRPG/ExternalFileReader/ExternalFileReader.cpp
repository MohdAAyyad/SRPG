// Fill out your copyright notice in the Description page of Project Settings.


#include "ExternalFileReader.h"



// Sets default values for this component's properties
UExternalFileReader::UExternalFileReader()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UExternalFileReader::BeginPlay()
{
	Super::BeginPlay();
	// arbitrary value that will be changed to the amount of tables we have
	tables.Reserve(20);
	firstTime = true;
	// ...
	
}


// Called every frame
void UExternalFileReader::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

void UExternalFileReader::AddRowToFighterTable(FName rowName_, int index_, FFighterTableStruct row_)
{
	
	if (tables[index_])
	{
		if (firstTime)
		{
			TArray<FName> rowNames;
			rowNames = tables[index_]->GetRowNames();
			for (auto n : rowNames)
			{
				tables[index_]->RemoveRow(n);
			}
			UE_LOG(LogTemp, Warning, TEXT("Deleted Equipped Fighter Table"));
			firstTime = false;
		}
		tables[index_]->AddRow(rowName_, row_);
		UE_LOG(LogTemp, Error, TEXT("Fighter Table Row Added!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Fighter Table returned NULL"));
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

