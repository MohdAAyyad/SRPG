// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemShop.h"
#include "Components/BoxComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Hub/HubWorldManager.h"
#include "Intermediary/Intermediate.h"

void AItemShop::LoadText()
{
	// leave this empty as we don't want text loading when we interact
}

void AItemShop::BeginPlay()
{
	// begin play
	Super::BeginPlay();

}

AItemShop::AItemShop()
{
	PrimaryActorTick.bCanEverTick = false;
}

TArray<UTexture*> AItemShop::GetItemTextureArray()
{
	return itemTextures;
}

TArray<UTexture*> AItemShop::GetEquipmentTextureArray()
{
	return equipmentTextures;
}

void AItemShop::BuyItem(int itemIndex_, int amountToBuy_)
{
	// will put a statement for price checking once money is integrated 


	// look for item index inside our table and add to the owned column
	FName converted = FName(*FString::FromInt(itemIndex_));
	FItemTableStruct row = fileReader->FindItemTableRow(converted, 0);
	if (Intermediate::GetInstance()->GetCurrentMoney() - row.price > 0)
	{
		Intermediate::GetInstance()->SpendMoney(row.price);
		fileReader->AddOwnedValueItemTable(converted, 0, amountToBuy_);

		UE_LOG(LogTemp, Warning, TEXT("Item Purchased!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough money to purchase this item"));
	}

}

void AItemShop::BuyEquipment(int equipmentIndex_, int amountToBuy_)
{
	FName converted = FName(*FString::FromInt(equipmentIndex_));
	FEquipmentTableStruct row = fileReader->FindEquipmentTableRow(converted, 1);
	// will put a statement for price checking once money is integrated
	if (Intermediate::GetInstance()->GetCurrentMoney() - row.value > 0)
	{
		fileReader->AddOwnedValueEquipmentTable(converted, 1, amountToBuy_);

		UE_LOG(LogTemp, Warning, TEXT("Equipment Purchased!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough money to purchase this"));
	}
	// need to find a way to do prices before I go about adding in price check

	// if (Intermediate::GetInstance()->GetCurrentMoney());
	// look for equipment index inside our table and add to the owned column


}

int AItemShop::GetWorldLevel()
{
	//will fill in functionality later
	if (hub)
	{
		return hub->GetHubWorldLevel();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Hub Manager NULL"));
		return 0;
	}
}

FString AItemShop::GetItemName(int itemIndex_)
{
	// return the item name from the values input 
	FName converted = FName(*FString::FromInt(itemIndex_));
	FItemTableStruct row = fileReader->FindItemTableRow(converted, 0);
	//UE_LOG(LogTemp, Warning, TEXT("GotItemName!"))
	if (row.level <= GetWorldLevel())
	{
		return row.name;
	}
	else
	{
		return FString("World Level Too high to sell this item");
	}
}

int AItemShop::GetItemStatIndex(int index_)
{
	// get the row and return the stat index
	FName converted = FName(*FString::FromInt(index_));
	FItemTableStruct row = fileReader->FindItemTableRow(converted, 0);
	if (row.level <= GetWorldLevel())
	{
		return row.statIndex;
	}
	else
	{
		return 0;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Got Stat Index!"));

}

FString AItemShop::GetEquipmentName(int itemIndex_)
{
	FName converted = FName(*FString::FromInt(itemIndex_));
	FEquipmentTableStruct row = fileReader->FindEquipmentTableRow(converted, 1);
	if (row.level <= GetWorldLevel())
	{
		return row.name;
	}
	else
	{
		return FString("World Level Too high to sell this item");
	}
	//UE_LOG(LogTemp, Warning, TEXT("Got Equipment Name!"));
}

TArray<FItemTableStruct> AItemShop::GetAllAvailbleItems(int worldLevel_)
{

	TArray<FItemTableStruct> result = fileReader->GetAllItems(fileReader->FindTableIndexInArray(FName("ItemTableStruct")), worldLevel_);
	return result;

}

TArray<FEquipmentTableStruct> AItemShop::GetAllAvailbleEquipment(int worldLevel_)
{
	TArray<FEquipmentTableStruct> result = fileReader->GetAllEquipment(fileReader->FindTableIndexInArray(FName("EquipmentTableStruct")), worldLevel_);
	return result;
}

