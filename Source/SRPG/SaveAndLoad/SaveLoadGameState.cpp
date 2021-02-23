// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveLoadGameState.h"
#include "../Intermediary/Intermediate.h"
#include "../SRPGGameMode.h"
#include "Engine/World.h"
#include "Definitions.h"
#include "SaveLoadGameState.h"
#include "SaveGameSystem.h"

ASaveLoadGameState::ASaveLoadGameState()
{
	bWillNeedToSwitchLevelAfterSaving = false;
}

#pragma region Save

void ASaveLoadGameState::SaveDuringLoadingScreenFromBattleToHub()
{
	Intermediate::GetInstance()->ReadyDataForSavingAfterBattle();
	bWillNeedToSwitchLevelAfterSaving = true;
	SaveData();
}
void ASaveLoadGameState::InitialSave()
{
	Intermediate::GetInstance()->GoBackToInitialValues();
	USRPGSaveGame* saveInstance_ = Cast<USRPGSaveGame>(UGameplayStatics::CreateSaveGameObject(USRPGSaveGame::StaticClass()));
	if (saveInstance_)
	{
		saveInstance_->saveSlotName = TEXT("Save0");
		saveInstance_->userIndex = 0;
		saveInstance_->currentRecruitedFightersState = TArray<FFighterTableStruct>();
		saveInstance_->currentWeaponsState = TArray<FEquipmentTableStruct>();
		saveInstance_->currentArmorsState = TArray<FEquipmentTableStruct>();
		saveInstance_->currentAccessoriesState = TArray<FEquipmentTableStruct>();
		saveInstance_->currentItemsState = TArray<FItemTableStruct>();
		saveInstance_->currentDayState = 1;
		saveInstance_->currentGoldState = 4000;
		saveInstance_->currentShardsState = 40;

		UGameplayStatics::AsyncSaveGameToSlot(saveInstance_, TEXT("Save0"), 0);
	}
}

void ASaveLoadGameState::SaveData()
{
	USRPGSaveGame* saveInstance_ = Cast<USRPGSaveGame>(UGameplayStatics::CreateSaveGameObject(USRPGSaveGame::StaticClass()));
	if (saveInstance_)
	{		
		saveInstance_->saveSlotName = TEXT("Save0");
		saveInstance_->userIndex = 0;
		saveInstance_->currentRecruitedFightersState = Intermediate::GetInstance()->GetLoadedDataObject().currentRecruitedFightersState;
		saveInstance_->currentWeaponsState = Intermediate::GetInstance()->GetLoadedDataObject().currentWeaponsState;
		saveInstance_->currentArmorsState = Intermediate::GetInstance()->GetLoadedDataObject().currentArmorsState;
		saveInstance_->currentAccessoriesState = Intermediate::GetInstance()->GetLoadedDataObject().currentAccessoriesState;
		saveInstance_->currentItemsState = Intermediate::GetInstance()->GetLoadedDataObject().currentItemsState;		
		saveInstance_->currentGoldState = Intermediate::GetInstance()->GetLoadedDataObject().currentGoldState;
		saveInstance_->currentShardsState = Intermediate::GetInstance()->GetLoadedDataObject().currentShardsState;
		saveInstance_->currentDayState = Intermediate::GetInstance()->GetLoadedDataObject().currentDayState;

		FAsyncSaveGameToSlotDelegate saveDelegate;
		saveDelegate.BindUObject(this, &ASaveLoadGameState::HandleSavingFinish);
		UGameplayStatics::AsyncSaveGameToSlot(saveInstance_, TEXT("Save0"), 0, saveDelegate);

	}
}

void ASaveLoadGameState::HandleSavingFinish(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	UE_LOG(LogTemp, Warning, TEXT("Finished saving"));
	if (bWillNeedToSwitchLevelAfterSaving)
	{
		bWillNeedToSwitchLevelAfterSaving = false;
		FinishedSavingNowToHub();
	}
}

void ASaveLoadGameState::FinishedSavingNowToHub()
{
	ASRPGGameMode* gameMode = Cast<ASRPGGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode)
	{
		if (Intermediate::GetInstance()->GetVictory())
		{
			gameMode->SwitchLevel(MAP_HUB);
		}
		else
		{
			gameMode->SwitchLevel(MAP_DayMenu);
		}
	}
		
}

#pragma endregion

#pragma region Load
void ASaveLoadGameState::LoadData()
{
	//Called from the level blueprint of the loading screen
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	LoadedDelegate.BindUObject(this, &ASaveLoadGameState::UpdateTheIntermediate);

	UGameplayStatics::AsyncLoadGameFromSlot(TEXT("Save0"), 0, LoadedDelegate);
}

void ASaveLoadGameState::UpdateTheIntermediate(const FString& slotName_, const int32 userIndex_, USaveGame* saveRef_)
{
	//Called after the save load object finishes loading the data
	FLoadData loaded_ = FLoadData();
	USRPGSaveGame* loadInstance_ = Cast<USRPGSaveGame>(saveRef_);
	if (loadInstance_)
	{
		loaded_.currentDayState = loadInstance_->currentDayState;
		loaded_.currentGoldState = loadInstance_->currentGoldState;
		loaded_.currentShardsState = loadInstance_->currentShardsState;
		loaded_.currentRecruitedFightersState = loadInstance_->currentRecruitedFightersState;
		loaded_.currentWeaponsState = loadInstance_->currentWeaponsState;
		loaded_.currentArmorsState = loadInstance_->currentArmorsState;
		loaded_.currentAccessoriesState = loadInstance_->currentAccessoriesState;
		loaded_.currentItemsState = loadInstance_->currentItemsState;

		Intermediate::GetInstance()->SetLoadedDataObject(loaded_);
		Intermediate::GetInstance()->SetHasLoadedData(true);
		Intermediate::GetInstance()->SetCurrentMoney(loaded_.currentGoldState);
		Intermediate::GetInstance()->SetCurrentShards(loaded_.currentShardsState);
		Intermediate::GetInstance()->SetCurrentDay(loaded_.currentDayState);
	}

	//All the data has been moved to the intermediate so move to the hub map
	ASRPGGameMode* gameMode = Cast<ASRPGGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode)
		gameMode->SwitchLevel(MAP_HUB);
}
#pragma endregion

