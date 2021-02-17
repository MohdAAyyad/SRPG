// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveLoadGameState.h"
#include "SRPGSaveGame.h"
#include "../Intermediary/Intermediate.h"
#include "../SRPGGameMode.h"
#include "Engine/World.h"
#include "Definitions.h"

ASaveLoadGameState::ASaveLoadGameState()
{
	saveLoadObj = NewObject<USRPGSaveGame>();
}

#pragma region Save

void ASaveLoadGameState::SaveDuringLoadingScreenFromBattleToHub()
{
	Intermediate::GetInstance()->ReadyDataForSavingAfterBattle();
	saveLoadObj->currentRecruitedFightersState = Intermediate::GetInstance()->GetLoadedDataObject().currentRecruitedFightersState;
	saveLoadObj->currentWeaponsState = Intermediate::GetInstance()->GetLoadedDataObject().currentWeaponsState;
	saveLoadObj->currentArmorsState = Intermediate::GetInstance()->GetLoadedDataObject().currentArmorsState;
	saveLoadObj->currentAccessoriesState = Intermediate::GetInstance()->GetLoadedDataObject().currentAccessoriesState;
	saveLoadObj->currentItemsState = Intermediate::GetInstance()->GetLoadedDataObject().currentItemsState;
	saveLoadObj->currentGoldState = Intermediate::GetInstance()->GetLoadedDataObject().currentGoldState;
	saveLoadObj->currentShardsState = Intermediate::GetInstance()->GetLoadedDataObject().currentShardsState;
	saveLoadObj->currentDayState = Intermediate::GetInstance()->GetLoadedDataObject().currentDayState;
	saveLoadObj->bWillNeedToSwitchLevelAfterSaving = true;
	saveLoadObj->SaveCurrentGameState();
}

void ASaveLoadGameState::SaveNewRecruitedFighters(TArray<FFighterTableStruct> fighters_)
{
	if (saveLoadObj)
	{
		saveLoadObj->currentRecruitedFightersState = fighters_;
	}
}
void ASaveLoadGameState::SaveNewWeapons(TArray<FEquipmentTableStruct> equip_)
{
	if (saveLoadObj)
	{
		saveLoadObj->currentWeaponsState = equip_;
	}
}

void ASaveLoadGameState::SaveNewArmors(TArray<FEquipmentTableStruct> equip_)
{
	if (saveLoadObj)
	{
		saveLoadObj->currentArmorsState = equip_;
	}
}
void ASaveLoadGameState::SaveNewAccessories(TArray<FEquipmentTableStruct> equip_)
{
	if (saveLoadObj)
	{
		saveLoadObj->currentAccessoriesState = equip_;
	}
}

void ASaveLoadGameState::SaveNewItems(TArray<FItemTableStruct> items_)
{
	if (saveLoadObj)
	{
		saveLoadObj->currentItemsState = items_;
	}
}
void ASaveLoadGameState::SaveNewGold(uint32 gold_)
{
	if (saveLoadObj)
	{
		saveLoadObj->currentGoldState = gold_;
	}
}
void ASaveLoadGameState::SaveNewShards(uint32 shards_)
{
	if (saveLoadObj)
	{
		saveLoadObj->currentShardsState = shards_;
	}
}
void ASaveLoadGameState::SaveNewDay(uint32 day_)
{
	if (saveLoadObj)
	{
		saveLoadObj->currentDayState = day_;
	}
}

void ASaveLoadGameState::InitialSave()
{
	Intermediate::GetInstance()->GoBackToInitialValues();
	if (saveLoadObj)
	{
		saveLoadObj->InitialSave();
	}
}

void ASaveLoadGameState::SaveData()
{
	if (saveLoadObj)
	{
		UE_LOG(LogTemp, Warning, TEXT("Yeah there's a saveloadobj"));
		saveLoadObj->currentRecruitedFightersState = Intermediate::GetInstance()->GetLoadedDataObject().currentRecruitedFightersState;
		saveLoadObj->currentWeaponsState = Intermediate::GetInstance()->GetLoadedDataObject().currentWeaponsState;
		saveLoadObj->currentArmorsState = Intermediate::GetInstance()->GetLoadedDataObject().currentArmorsState;
		saveLoadObj->currentAccessoriesState = Intermediate::GetInstance()->GetLoadedDataObject().currentAccessoriesState;
		saveLoadObj->currentItemsState = Intermediate::GetInstance()->GetLoadedDataObject().currentItemsState;
		saveLoadObj->currentGoldState = Intermediate::GetInstance()->GetLoadedDataObject().currentGoldState;
		saveLoadObj->currentShardsState = Intermediate::GetInstance()->GetLoadedDataObject().currentShardsState;
		saveLoadObj->currentDayState = Intermediate::GetInstance()->GetLoadedDataObject().currentDayState;
		saveLoadObj->SaveCurrentGameState();
	}

}

void ASaveLoadGameState::FinishedSavingNowToHub(const FString& SlotName, const int32 UserIndex, bool bSuccess)
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
	if (saveLoadObj)
	{
		saveLoadObj->gameState = this;
		saveLoadObj->InitLoad();
	}
}

void ASaveLoadGameState::UpdateTheIntermediate()
{
	//Called after the save load object finishes loading the data
	if (saveLoadObj)
	{
		FLoadData loaded_ = FLoadData();

		loaded_.currentDayState = saveLoadObj->currentDayState;
		loaded_.currentGoldState = saveLoadObj->currentGoldState;
		loaded_.currentShardsState = saveLoadObj->currentShardsState;
		loaded_.currentRecruitedFightersState = saveLoadObj->currentRecruitedFightersState;
		loaded_.currentWeaponsState = saveLoadObj->currentWeaponsState;
		loaded_.currentArmorsState = saveLoadObj->currentArmorsState;
		loaded_.currentAccessoriesState = saveLoadObj->currentAccessoriesState;
		loaded_.currentItemsState = saveLoadObj->currentItemsState;

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

