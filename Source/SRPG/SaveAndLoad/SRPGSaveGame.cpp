// Fill out your copyright notice in the Description page of Project Settings.


#include "SRPGSaveGame.h"
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
	currentDayState = 1;
	currentGoldState = 4000;
	currentShardsState = 40;
}