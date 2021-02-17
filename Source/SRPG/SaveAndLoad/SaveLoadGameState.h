// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "../ExternalFileReader/FighterTableStruct.h"
#include "../ExternalFileReader/EquipmentTableStruct.h"
#include "../ExternalFileReader/ItemTableStruct.h"
#include "SaveLoadGameState.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API ASaveLoadGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASaveLoadGameState();

	void UpdateTheIntermediate(); //Called from the save/load object once loading has finished

	void SaveNewRecruitedFighters(TArray<FFighterTableStruct> fighters_);
	void SaveNewWeapons(TArray<FEquipmentTableStruct> equip_);
	void SaveNewArmors(TArray<FEquipmentTableStruct> equip_);
	void SaveNewAccessories(TArray<FEquipmentTableStruct> equip_);
	void SaveNewItems(TArray<FItemTableStruct> items_);
	void SaveNewGold(uint32 gold_);
	void SaveNewShards(uint32 shards_);
	void SaveNewDay(uint32 day_);

	UFUNCTION(BlueprintCallable)
		void SaveData();
	UFUNCTION(BlueprintCallable)
		void SaveDuringLoadingScreenFromBattleToHub();

	UFUNCTION()
		void FinishedSavingNowToHub(const FString& SlotName, const int32 UserIndex, bool bSuccess);
protected:
	class USRPGSaveGame* saveLoadObj;

	UFUNCTION(BlueprintCallable) //Called from the level blueprint
		void LoadData();

	UFUNCTION(BlueprintCallable)
		void InitialSave(); //When you start the game

	
	
};
