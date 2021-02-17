// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "../ExternalFileReader/FighterTableStruct.h"
#include "../ExternalFileReader/EquipmentTableStruct.h"
#include "../ExternalFileReader/ItemTableStruct.h"
#include "SRPGSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API USRPGSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	USRPGSaveGame();

		void SaveCurrentGameState();
		void InitLoad();
	

		bool bWillNeedToSwitchLevelAfterSaving; //When true, tells to move to hub world
	
		void InitialSave(); //When you start the game

	UPROPERTY(VisibleAnywhere, Category = Basic)
		FString saveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
		uint32 userIndex;

	UPROPERTY(VisibleAnywhere, Category = Basic)
		TArray<FFighterTableStruct> currentRecruitedFightersState;
	UPROPERTY(VisibleAnywhere, Category = Basic)
		TArray<FEquipmentTableStruct> currentWeaponsState;
	UPROPERTY(VisibleAnywhere, Category = Basic)
		TArray<FEquipmentTableStruct> currentArmorsState;
	UPROPERTY(VisibleAnywhere, Category = Basic)
		TArray<FEquipmentTableStruct> currentAccessoriesState;
	UPROPERTY(VisibleAnywhere, Category = Basic)
		TArray<FItemTableStruct> currentItemsState;
	UPROPERTY(VisibleAnywhere, Category = Basic)
		uint32 currentDayState;
	UPROPERTY(VisibleAnywhere, Category = Basic)
		uint32 currentGoldState;
	UPROPERTY(VisibleAnywhere, Category = Basic)
		uint32 currentShardsState;

		ASaveLoadGameState* gameState;

		void FinishedSaving(const FString& SlotName, const int32 UserIndex_, bool bSuccess);

		void LoadEveryStateDelegate(const FString& slotName_, const int32 userIndex_, USaveGame* saveRef_);




	
	
};
