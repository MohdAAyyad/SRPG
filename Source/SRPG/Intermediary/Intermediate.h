// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <memory.h>
#include "../ExternalFileReader/FOpponentStruct.h"
#include "../ExternalFileReader/FighterTableStruct.h"
#include "../ExternalFileReader/EquipmentTableStruct.h"
#include "../ExternalFileReader/ItemTableStruct.h"

/**
 * 
 */

struct SRPG_API FLoadData
{
public:
	FLoadData()
	{
		currentRecruitedFightersState = TArray<FFighterTableStruct>();
		currentWeaponsState = TArray<FEquipmentTableStruct>();
		currentArmorsState = TArray<FEquipmentTableStruct>();
		currentAccessoriesState = TArray<FEquipmentTableStruct>();
		currentItemsState = TArray<FItemTableStruct>();
		currentDayState = 1;
		currentGoldState = 4000;
		currentShardsState = 40;
	}

	TArray<FFighterTableStruct> currentRecruitedFightersState;
	TArray<FEquipmentTableStruct> currentWeaponsState;
	TArray<FEquipmentTableStruct> currentArmorsState;
	TArray<FEquipmentTableStruct> currentAccessoriesState;
	TArray<FItemTableStruct> currentItemsState;
	uint32 currentDayState;
	uint32 currentGoldState;
	uint32 currentShardsState;
};

class SRPG_API Intermediate
{
public:
	Intermediate();
	~Intermediate();

protected:

	int protagonistLevel;
	TArray<FFighterTableStruct> selectedFighters; //The indexes of the fighters that are going to be used in the next battle
	TArray<int> unitsOnHold; //When the player engages in activities that use units, these units become on hold. If the activity fails, these units are removed from the currentRoster. The array saves the index of the units in the currentRoster array.
	TArray<int> deadUnitsIDs; //Stores the ids of dead units. Checked by the fighter shop on begin play.
	int maxRosterSize;
	int currentRosterSize;
	int currentDeploymentSize;
	FOpponentStruct nextOpponent; //Value is passed on to the battle manager which passes it onto the enemy manager 
							//to spawn the enemies.Used for enemy stat decrease activities 	
	int enemyStatDecreaseValue;
	int enemyStatDecreaseIndex;
	int latestFighterID;// Used to make sure that fighters all have a unique ID

	static TUniquePtr<Intermediate, TDefaultDelete<Intermediate>> instance;

	int affectedParty = -1;
	int statIndex = -1;
	float changeCrowdValue = 0;

	bool bRewardsWereGiven; //Checked the hub world manager general UI on begin play to see if a notification should be played

	FLoadData loadedData;
	bool bHasLoadedData;
	bool bVictory;

public:

	void Victory(int moneyReward_, int shardsReward_); //Adds 1 to the story progress //Called from the battle manager
	void Defeat(int moneyCompensation_, int shardsCompensation_);
	int GetCurrentDay();
	int GetProtagonistLevel();
	void SetProtagonistLevel(int value_);
	void UpdateCurrentRosterSize(int value_); //Value_ can be + or -. 
	int GetCurrentRosterSize();
	int GetMaxRosterSize();
	int GetLatestFighterID();
	void SetLatestFighterID(int id_);
	int GetCurrentDeploymentSize();
	int GetCurrentMoney();
	int GetCurrentShards();
	void SetCurrentMoney(int money_); //Called when loading in
	void SetCurrentShards(int shards_);
	void SetCurrentDay(int day_);
	void AddFighterToSelected(FFighterTableStruct fighter_);
	void ResetSelectedFighters();
	TArray<FFighterTableStruct> GetSelectedFighters(); //Called by the battle manager
	void SpendMoney(int money_);
	void AddMoney(int money_);
	void SpendShards(int shards_);
	void SetNextOpponent(FOpponentStruct op_); //Called by transition to battle when the player collides with it and ends the day. 
										 //Determines the next fight when the next fight is not a story one.
	FOpponentStruct GetNextOpponent(); //Called by enemy manager to know the details of the next opponent.
	void ImprovePlayerCRD(float value_);

	TArray<int>& GetDeadUnits();
	void PushUnitToDead(int unitId_);

	static Intermediate* GetInstance();

	void ChangeStats(int partyIndex_, int statIndex_);
	int GetStatsChange(int partyIndex_);
	int GetAffecteParty();
	void ResetChangeStats(); //Make sure change stats variables are reset

	bool GetRewardsWereGiven();
	void ResetRewardsWereGiven();

	FLoadData& GetLoadedDataObject();
	void SetLoadedDataObject(FLoadData obj_);

	bool GetHasLoadedData();
	void SetHasLoadedData(bool value_);

	bool GetVictory();

	void ReadyDataForSavingAfterBattle(); //Removes dead fighters from the loaded list of fighters and levels up surviving fighters
};