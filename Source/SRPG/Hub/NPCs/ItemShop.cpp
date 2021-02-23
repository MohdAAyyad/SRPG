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
#include "../../SaveAndLoad/SaveLoadGameState.h"


AItemShop::AItemShop() :ANPC()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AItemShop::BeginPlay()
{
	Super::BeginPlay();

	if (Intermediate::GetInstance()->GetHasLoadedData())
	{
		LoadEquipmentAndItems();
	}
}

/*
Money check is always handled in the blueprints because we need to give the player a notification that they cannot buy stuff now

*/

void AItemShop::BuyItem(int itemId_, int amountToBuy_, int price_)
{
	Intermediate::GetInstance()->SpendShards(price_ * amountToBuy_);
	if (fileReader)
	{
		fileReader->AddOwnedValueItemTable(0, itemId_, amountToBuy_);
	}

}

void AItemShop::BuyEquipment(int equipmentIndex_, int equipId_, int amountToBuy_, int price_)
{
	Intermediate::GetInstance()->SpendShards(price_ * amountToBuy_);
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
	if (hubManager)
	{
		return hubManager->GetHubWorldLevel();
	}
	
	return -1;
}

int AItemShop::GetCurrentMoney()
{
	if (hubManager)
	{
		return hubManager->GetCurrentMoney();
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

						GetWorld()->GetTimerManager().SetTimer(timeToAddWidgetHandle, this, &ANPC::DelayedAddWidgetToViewPort, focusRate + 0.1f, false);
						ctrl->SetInputMode(FInputModeUIOnly());
					}
				}

			}
		}
	}
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

		if (armor.Num() > 0)
		{
			for (auto a : armor)
			{
				allWpnsAndArmorAndAcc.Push(a);
			}
			armor.Empty();
		}

	

		armor = fileReader->FindAllOwnedEquipment(3); //Acc

		if (armor.Num() > 0)
		{
			for (auto a : armor)
			{
				allWpnsAndArmorAndAcc.Push(a);
			}
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

void AItemShop::SaveNewEquipment() //Called from the UI when you leave the shop after buying equipment
{
	TArray<FEquipmentTableStruct> weapons_ = fileReader->FindAllOwnedEquipment(1);
	TArray<FEquipmentTableStruct>armors_ = fileReader->FindAllOwnedEquipment(2);
	TArray<FEquipmentTableStruct>acc_ = fileReader->FindAllOwnedEquipment(3);
	UE_LOG(LogTemp, Warning, TEXT("Got the equipment from FR"));
	//Equipment will be passed on to the save object when we end the day
	Intermediate::GetInstance()->loadedData.currentWeaponsState = weapons_;
	Intermediate::GetInstance()->loadedData.currentArmorsState = armors_;
	Intermediate::GetInstance()->loadedData.currentAccessoriesState = acc_;
	UE_LOG(LogTemp, Warning, TEXT("Update the intermediate's equipment"));

	/*ASaveLoadGameState* gameState_ = Cast<ASaveLoadGameState>(GetWorld()->GetGameState());
	if (gameState_)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found the game state"));
		gameState_->SaveNewWeapons(weapons_);
		gameState_->SaveNewArmors(armors_);
		gameState_->SaveNewAccessories(acc_);
	}
	*/
}

void AItemShop::SaveNewItems() //Called from the UI when you leave the shop after buying items
{
	//Items will be passed onto the save object when we save
	TArray<FItemTableStruct> items_ = fileReader->GetAllOwnedItems(0);
	Intermediate::GetInstance()->loadedData.currentItemsState = items_;

  /* ASaveLoadGameState* gameState_ = Cast<ASaveLoadGameState>(GetWorld()->GetGameState());
	if (gameState_)
	{
		gameState_->SaveNewItems(items_);
	}
	*/
}

void AItemShop::LoadEquipmentAndItems()
{
	TArray<FItemTableStruct> items_ = Intermediate::GetInstance()->loadedData.currentItemsState;
	TArray<FEquipmentTableStruct> weapons_ = Intermediate::GetInstance()->loadedData.currentWeaponsState;
	TArray<FEquipmentTableStruct> armors_ = Intermediate::GetInstance()->loadedData.currentArmorsState;
	TArray<FEquipmentTableStruct> accessories_ = Intermediate::GetInstance()->loadedData.currentAccessoriesState;

	if (Intermediate::GetInstance()->loadedData.currentItemsState.Num() > 0)
		Intermediate::GetInstance()->loadedData.currentItemsState.Empty();
	if (Intermediate::GetInstance()->loadedData.currentWeaponsState.Num() > 0)
		Intermediate::GetInstance()->loadedData.currentWeaponsState.Empty();
	if (Intermediate::GetInstance()->loadedData.currentArmorsState.Num() > 0)
		Intermediate::GetInstance()->loadedData.currentArmorsState.Empty();
	if (Intermediate::GetInstance()->loadedData.currentAccessoriesState.Num() > 0)
		Intermediate::GetInstance()->loadedData.currentAccessoriesState.Empty();

	for (int i = 0; i < items_.Num(); i++)
	{
		fileReader->UpdateOwnedValueItemTableAfterLoad(0, items_[i].id, items_[i].owned);
	}

	for (int i = 0; i < weapons_.Num(); i++)
	{
		fileReader->UpdateOwnedValueEquipmentTableAfterLoad(1, weapons_[i].id, weapons_[i].owned);
	}
	for (int i = 0; i < armors_.Num(); i++)
	{
		fileReader->UpdateOwnedValueEquipmentTableAfterLoad(2, armors_[i].id, armors_[i].owned);
	}
	for (int i = 0; i < accessories_.Num(); i++)
	{
		fileReader->UpdateOwnedValueEquipmentTableAfterLoad(3, accessories_[i].id, accessories_[i].owned);
	}
}

//The data we're saving only includes the owned items, so when we load we should be adding the owned value to the table.

//TODO
/*
* Check bHasLoaded in the intermediate and if true, load the data
* Save the day data after loading into the hub world
* Save the fighters data after finishing a battle
* Save everything that's changed after exiting the pause menu

*/

