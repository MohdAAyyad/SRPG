// Fill out your copyright notice in the Description page of Project Settings.


#include "SRPGSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "SaveLoadGameState.h"
#include "SaveGameSystem.h"

USRPGSaveGame::USRPGSaveGame()
{
	saveSlotName = TEXT("Save0");
	userIndex = 0;
	currentRecruitedFightersState = TArray<FFighterTableStruct>();
	currentWeaponsState = TArray<FEquipmentTableStruct>();
	currentArmorsState = TArray<FEquipmentTableStruct>();
	currentAccessoriesState = TArray<FEquipmentTableStruct>();
	currentItemsState = TArray<FItemTableStruct>();
	currentDayState = currentGoldState = currentShardsState = 0;
	gameState = nullptr;
	bWillNeedToSwitchLevelAfterSaving = false;
}

#pragma region Save
void USRPGSaveGame::SaveCurrentGameState()
{
	FAsyncSaveGameToSlotDelegate saveDelegate = FAsyncSaveGameToSlotDelegate();
	USRPGSaveGame* saveInstance_ = Cast<USRPGSaveGame>(UGameplayStatics::CreateSaveGameObject(USRPGSaveGame::StaticClass()));
	if (saveInstance_)
	{
		
		saveInstance_->currentRecruitedFightersState = currentRecruitedFightersState;
		saveInstance_->currentWeaponsState = currentWeaponsState;
		saveInstance_->currentArmorsState = currentArmorsState;
		saveInstance_->currentAccessoriesState = currentAccessoriesState;
		saveInstance_->currentItemsState = currentItemsState;
		saveInstance_->currentDayState = currentDayState;
		saveInstance_->currentGoldState = currentGoldState;
		saveInstance_->currentShardsState = currentShardsState;

		/*UE_LOG(LogTemp, Warning, TEXT("In save object currentDayState %d "), saveInstance_->currentDayState);
		UE_LOG(LogTemp, Warning, TEXT("In save object currentGoldState %d "), saveInstance_->currentGoldState);
		UE_LOG(LogTemp, Warning, TEXT("In save object currentShardsState %d "), saveInstance_->currentShardsState);
		UE_LOG(LogTemp, Warning, TEXT("In save object currentRecruitedFightersState %d "), saveInstance_->currentRecruitedFightersState.Num());
		UE_LOG(LogTemp, Warning, TEXT("In save object currentWeaponsState %d "), saveInstance_->currentWeaponsState.Num());
		UE_LOG(LogTemp, Warning, TEXT("In save object currentArmorsState %d "), saveInstance_->currentArmorsState.Num());
		UE_LOG(LogTemp, Warning, TEXT("In save object currentAccessoriesState %d "), saveInstance_->currentAccessoriesState.Num());
		UE_LOG(LogTemp, Warning, TEXT("In save object currentItemsState %d "), saveInstance_->currentItemsState.Num());*/

		if (!bWillNeedToSwitchLevelAfterSaving)
		{
			//Delete the current version of the save game first
			if(UGameplayStatics::DoesSaveGameExist((TEXT("%sSaveGames/%s.sav"), *FPaths::ProjectSavedDir(), saveSlotName), userIndex))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Save game exists"));
				if (UGameplayStatics::DeleteGameInSlot((TEXT("%sSaveGames/%s.sav"), *FPaths::ProjectSavedDir(), saveSlotName), userIndex))
				{
					//UE_LOG(LogTemp, Warning, TEXT("Deleted save game"));
				}
			}
			//Write it again
			UGameplayStatics::AsyncSaveGameToSlot(saveInstance_, saveSlotName, userIndex, FAsyncSaveGameToSlotDelegate());
		}
		else
		{
			//Delete the current version of the save game first
			if (UGameplayStatics::DoesSaveGameExist((TEXT("%sSaveGames/%s.sav"), *FPaths::ProjectSavedDir(), saveSlotName), userIndex))
			{
				UGameplayStatics::DeleteGameInSlot((TEXT("%sSaveGames/%s.sav"), *FPaths::ProjectSavedDir(), saveSlotName), userIndex);
			}
			//Tell the game instance that you've finished saving
			bWillNeedToSwitchLevelAfterSaving = false;
			saveDelegate.BindUObject(gameState, &ASaveLoadGameState::FinishedSavingNowToHub);
			UGameplayStatics::AsyncSaveGameToSlot(saveInstance_, saveSlotName, userIndex, saveDelegate);

		}

	}
}

void USRPGSaveGame::InitialSave()
{
	USRPGSaveGame* saveInstance_ = Cast<USRPGSaveGame>(UGameplayStatics::CreateSaveGameObject(USRPGSaveGame::StaticClass()));
	if (saveInstance_)
	{
		if (UGameplayStatics::DoesSaveGameExist((TEXT("%sSaveGames/%s.sav"), *FPaths::ProjectSavedDir(), saveSlotName), userIndex))
		{
			UGameplayStatics::DeleteGameInSlot((TEXT("%sSaveGames/%s.sav"), *FPaths::ProjectSavedDir(), saveSlotName), userIndex);
		}
		UGameplayStatics::AsyncSaveGameToSlot(saveInstance_, saveSlotName, userIndex, FAsyncSaveGameToSlotDelegate());
	}
}

void USRPGSaveGame::FinishedSaving(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	if (bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("BSUCCESS"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BFAILURE"));
	}
}

#pragma endregion

#pragma region Load
void USRPGSaveGame::InitLoad()
{
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	LoadedDelegate.BindUObject(this, &USRPGSaveGame::LoadEveryStateDelegate);

	UGameplayStatics::AsyncLoadGameFromSlot(saveSlotName, userIndex, LoadedDelegate);
}
void USRPGSaveGame::LoadEveryStateDelegate(const FString& slotName_, const int32 userIndex_, USaveGame* saveRef_)
{
	USRPGSaveGame* instance_ = Cast<USRPGSaveGame>(saveRef_);
	if (instance_)
	{
		currentRecruitedFightersState = instance_->currentRecruitedFightersState;
		currentWeaponsState = instance_->currentWeaponsState;
		currentArmorsState = instance_->currentArmorsState;
		currentAccessoriesState = instance_->currentAccessoriesState;
		currentItemsState = instance_->currentItemsState;
		currentDayState = instance_->currentDayState;
		currentGoldState = instance_->currentGoldState;
		currentShardsState = instance_->currentShardsState;
		//Loading is done, tell the game state so it updates the intermediate
		if (gameState)
			gameState->UpdateTheIntermediate();
	}
}
#pragma endregion