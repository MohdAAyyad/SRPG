// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemShop.h"
#include "Components/BoxComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

void AItemShop::LoadText()
{
	// leave this empty as we don't want text loading when we interact
}

void AItemShop::BeginPlay()
{
	// begin play
	Super::BeginPlay();
}

void AItemShop::BuyItem(int itemIndex_, int amountToBuy_)
{
	// will put a statement for price checking once money is integrated 

	// look for item index inside our table and add to the owned column
	FName converted = FName(*FString::FromInt(itemIndex_));
	fileReader->AddOwnedValueItemTable(converted, 0, amountToBuy_);

	UE_LOG(LogTemp, Warning, TEXT("Item Purchased!"));
}

void AItemShop::BuyEquipment(int equipmentIndex_, int amountToBuy_)
{
	// will put a statement for price checking once money is integrated

	// look for equipment index inside our table and add to the owned column
	FName converted = FName(*FString::FromInt(equipmentIndex_));
	fileReader->AddOwnedValueEquipmentTable(converted, 1, amountToBuy_);

	UE_LOG(LogTemp, Warning, TEXT("Equipment Purchased!"));
}

int AItemShop::GetWorldLevel()
{
	//will fill in functionality later
	return 0;
}

FString AItemShop::GetItemName(int itemIndex_)
{
	// return the item name from the values input 
	FName converted = FName(*FString::FromInt(itemIndex_));
	FItemTableStruct row = fileReader->FindItemTableRow(converted, 0);
	//UE_LOG(LogTemp, Warning, TEXT("GotItemName!"));
	return row.name;
}

int AItemShop::GetItemStatIndex(int index_)
{
	// get the row and return the stat index
	FName converted = FName(*FString::FromInt(index_));
	FItemTableStruct row = fileReader->FindItemTableRow(converted, 0);
	//UE_LOG(LogTemp, Warning, TEXT("Got Stat Index!"));
	return row.statIndex;
}

FString AItemShop::GetEquipmentName(int itemIndex_)
{
	FName converted = FName(*FString::FromInt(itemIndex_));
	FEquipmentTableStruct row = fileReader->FindEquipmentTableRow(converted, 1);
	//UE_LOG(LogTemp, Warning, TEXT("Got Equipment Name!"));
	return row.name;
}
