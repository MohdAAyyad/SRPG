// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemShop.h"
#include "Components/BoxComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Hub/HubWorldManager.h"
#include "Intermediary/Intermediate.h"
#include "SRPGCharacter.h"
#include "SRPGPlayerController.h"
#include "TimerManager.h"


AItemShop::AItemShop() :ANPC()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AItemShop::BeginPlay()
{
	Super::BeginPlay();
}

/*
Money check is always handled in the blueprints because we need to give the player a notification that they cannot buy stuff now

*/

void AItemShop::BuyItem(int itemId_, int amountToBuy_, int price_)
{
	Intermediate::GetInstance()->SpendMoney(price_ * amountToBuy_);
	if (fileReader)
	{
		fileReader->AddOwnedValueItemTable(0, itemId_, amountToBuy_);
	}

}

void AItemShop::BuyEquipment(int equipmentIndex_, int equipId_, int amountToBuy_, int price_)
{
	Intermediate::GetInstance()->SpendMoney(price_ * amountToBuy_);
	int tableIndex = equipmentIndex_ + 1;
	if (fileReader)
	{
		fileReader->BuyEquipment(tableIndex, equipId_, amountToBuy_);
	}
}

void AItemShop::SellEquipment(int equipmentIndex_, int equipId_, int price_, int amountSold_)
{
	Intermediate::GetInstance()->SpendMoney( -price_ * amountSold_);
	int tableIndex = equipmentIndex_ + 1;
	if (fileReader)
	{
		fileReader->SellEquipment(tableIndex, equipId_, amountSold_);
	}
}

int AItemShop::GetWorldLevel()
{
	if (hub)
	{
		return hub->GetHubWorldLevel();
	}
	
	return -1;
}

int AItemShop::GetCurrentMoney()
{
	if (hub)
	{
		return hub->GetCurrentMoney();
	}

	return -1;
}

TArray<FItemTableStruct> AItemShop::GetAllAvailableItems(int worldLevel_)
{

	TArray<FItemTableStruct> result = fileReader->GetAllItemsConditionedByWorldLevel(0, worldLevel_);
	return result;

}

void AItemShop::SellItem(int itemID, int price_, int amountSold_)
{
	Intermediate::GetInstance()->SpendMoney(-price_ * amountSold_);
	if (fileReader)
		fileReader->SellItem(0, itemID, amountSold_);
}

TArray<FEquipmentTableStruct> AItemShop::GetAllAvailableEquipmentOfACertainType(int equipmentIndex_, int subIndex_)
{
	int tableIndex_ = equipmentIndex_ + 1; //1 for wpn 2 for armor 3 for acc
	if (fileReader)
	{
		return fileReader->GetAllEquipmentOfACertainTypeConditionedByWorldLevel(tableIndex_, equipmentIndex_, subIndex_, GetWorldLevel());
	}
	return TArray<FEquipmentTableStruct>();
}

void AItemShop::OnOverlapWithPlayer(UPrimitiveComponent * overlappedComp_, AActor * otherActor_,
	UPrimitiveComponent * otherComp_, int32 otherBodyIndex_,
	bool bFromSweepO, const FHitResult & sweepResult_)
{
	if (otherActor_ != nullptr && otherActor_ != this && overlappedComp_ != nullptr)
	{
		// check if we are being interacted with and process the logic 
		if (interactedWith)
		{
			ASRPGCharacter* player = Cast<ASRPGCharacter>(otherActor_);
			if (player)
			{
				playerRef = player;
				if (widget && widget->GetUserWidgetObject()->IsInViewport() == false)
				{
					//Focus the camera on the item shop
					ASRPGPlayerController* ctrl = Cast<ASRPGPlayerController>(GetWorld()->GetFirstPlayerController());
					if (ctrl)
					{
						float focusRate = 0.45f;
						ctrl->FocusOnThisNPC(this, focusRate);
						FTimerHandle timeToAddWidgetHandle;

						GetWorld()->GetTimerManager().SetTimer(timeToAddWidgetHandle, this, &AItemShop::DelayedAddWidgetToViewPort, focusRate + 0.1f, false);
						ctrl->SetInputMode(FInputModeUIOnly());
					}
				}

			}
		}
	}
}

void AItemShop::DelayedAddWidgetToViewPort()
{
	//By then the camera will have moved to focus on the item shop
	if(widget)
		if(widget->GetUserWidgetObject())
			widget->GetUserWidgetObject()->AddToViewport();
}

void AItemShop::LeaveNPC()
{
	if (widget)
		if (widget->GetUserWidgetObject())
			widget->GetUserWidgetObject()->RemoveFromViewport();


	ASRPGPlayerController* ctrl = Cast<ASRPGPlayerController>(GetWorld()->GetFirstPlayerController());
	if (ctrl)
	{
		if (playerRef)
		{
			ctrl->SetInputMode(FInputModeGameAndUI());
			ctrl->FocusOnThisNPC(playerRef, 0.45f);
			playerRef = nullptr;
		}
	}
}

TArray<FEquipmentTableStruct> AItemShop::GetAllOwnedEquipment()
{
	TArray<FEquipmentTableStruct> allWpnsAndArmorAndAcc;

	if (fileReader)
	{
		allWpnsAndArmorAndAcc = fileReader->FindAllOwnedEquipment(1); //Weapons
		
		TArray<FEquipmentTableStruct> armor = fileReader->FindAllOwnedEquipment(2); //Armor

		for (auto a : armor)
		{
			allWpnsAndArmorAndAcc.Push(a);
		}

		armor.Empty();

		armor = fileReader->FindAllOwnedEquipment(3); //Acc

		for (auto a : armor)
		{
			allWpnsAndArmorAndAcc.Push(a);
		}

	}

	return allWpnsAndArmorAndAcc;
}

TArray<FItemTableStruct> AItemShop::GetAllOwnedItems()
{
	if (fileReader)
	{
		return fileReader->GetAllOwnedItems(0);
	}

	return  TArray<FItemTableStruct>();
}

AHubWorldManager* AItemShop::GetHubWolrdManager()
{
	return hub;
}

