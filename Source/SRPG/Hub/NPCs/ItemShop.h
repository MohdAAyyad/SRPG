// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hub/NPC.h"
#include "ExternalFileReader/EquipmentTableStruct.h"
#include "ItemShop.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API AItemShop : public ANPC
{
	GENERATED_BODY()
protected:
	void BeginPlay() override;

public:
	AItemShop();

protected:
	// how much is the player planning to buy. resets after a new item is selected
	UFUNCTION(BlueprintCallable)
		void BuyItem(int itemId_, int amountToBuy_, int price_);
	UFUNCTION(BlueprintCallable)
		void BuyEquipment(int equipmentIndex_, int equipId_, int amountToBuy_, int price_);
	UFUNCTION(BlueprintCallable)
		void SellEquipment(int equipmentIndex_, int equipId_, int price_, int amountSold_);
	UFUNCTION(BlueprintCallable)
		void SellItem(int itemID, int price_, int amountSold_);
	UFUNCTION(BlueprintCallable)
		int GetWorldLevel();
	UFUNCTION(BlueprintCallable)
		TArray<FItemTableStruct> GetAllAvailableItems(int worldLevel_);
	UFUNCTION(BlueprintCallable)
		TArray<FEquipmentTableStruct> GetAllAvailableEquipmentOfACertainType(int equipmentIndex_, int subIndex_);

	UFUNCTION(BlueprintCallable)
		TArray<FEquipmentTableStruct> GetAllOwnedEquipment(); //Used when selling
	UFUNCTION(BlueprintCallable)
		TArray<FItemTableStruct> GetAllOwnedItems(); //Used when selling

	UFUNCTION(BlueprintCallable)
		int GetCurrentMoney();

		void LeaveNPC() override;



	void OnOverlapWithPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult) override;


};
