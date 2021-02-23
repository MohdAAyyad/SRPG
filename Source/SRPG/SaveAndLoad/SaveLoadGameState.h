// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "../ExternalFileReader/FighterTableStruct.h"
#include "../ExternalFileReader/EquipmentTableStruct.h"
#include "../ExternalFileReader/ItemTableStruct.h"
#include "SRPGSaveGame.h"
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

	void UpdateTheIntermediate(const FString& slotName_, const int32 userIndex_, USaveGame* saveRef_); //Called from the save/load object once loading has finished

	UFUNCTION(BlueprintCallable)
		void SaveData();
	UFUNCTION(BlueprintCallable)
		void SaveDuringLoadingScreenFromBattleToHub();

		void FinishedSavingNowToHub();
protected:

	UFUNCTION(BlueprintCallable) //Called from the level blueprint
		void LoadData();

	UFUNCTION(BlueprintCallable)
		void InitialSave(); //When you start the game

	bool bWillNeedToSwitchLevelAfterSaving;

	void HandleSavingFinish(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	
	
};
