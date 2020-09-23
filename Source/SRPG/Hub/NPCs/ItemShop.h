// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hub/NPC.h"
#include "ItemShop.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API AItemShop : public ANPC
{
	GENERATED_BODY()
protected:
	void LoadText() override;
	void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Hub")
	class AHubWorldManager* hub;
public:
	AItemShop();
	UPROPERTY(EditAnywhere)
	TArray<UTexture*> itemTextures;
	UPROPERTY(EditAnywhere)
	TArray<UTexture*> equipmentTextures;
	UFUNCTION(BlueprintCallable)
	TArray<UTexture*> GetItemTextureArray();
	UFUNCTION(BlueprintCallable)
	TArray<UTexture*> GetEquipmentTextureArray();
	// how much is the player planning to buy. resets after a new item is selected
	UFUNCTION(BlueprintCallable)
		void BuyItem(int itemIndex_, int amountToBuy_);
	UFUNCTION(BlueprintCallable)
		void BuyEquipment(int equipmentIndex_, int amountToBuy_);
	UFUNCTION(BlueprintCallable)
		int GetWorldLevel();
	UFUNCTION(BlueprintCallable)
		FString GetItemName(int itemIndex_);
	UFUNCTION(BlueprintCallable)
		int GetItemStatIndex(int index_);
	UFUNCTION(BlueprintCallable)
		FString GetEquipmentName(int itemIndex_);
	UFUNCTION(BlueprintCallable)
	TArray<FItemTableStruct> GetAllAvailbleItems(int worldLevel_);
	UFUNCTION(BlueprintCallable)
	TArray<FEquipmentTableStruct> GetAllAvailbleEquipment(int worldLevel_);

};
