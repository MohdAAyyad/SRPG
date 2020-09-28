// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <memory.h>
#include "../ExternalFileReader/FOpponentStruct.h"
#include "../ExternalFileReader/FighterTableStruct.h"

/**
 * 
 */
class SRPG_API Intermediate
{
public:
	Intermediate();
	~Intermediate();

protected:

	int storyProgress; //Determines whether the next fight is a story fight or not. Increased by 1 after every victory.
	int protagonistLevel;
	TArray<FFighterTableStruct> selectedFighters; //The indexes of the fighters that are going to be used in the next battle
	TArray<int> unitsOnHold; //When the player engages in activities that use units, these units become on hold. If the activity fails, these units are removed from the currentRoster. The array saves the index of the units in the currentRoster array.
	TArray<int> deadUnitsIDs; //Stores the ids of dead units. Checked by the fighter shop on begin play.
	int maxRosterSize;
	int currentRosterSize;
	int maxDeploymentSize;
	int currentDeploymentSize;
	int currentMoney;
	FOpponentStruct nextOpponent; //Value is passed on to the battle manager which passes it onto the enemy manager 
							//to spawn the enemies.Used for enemy stat decrease activities 	
	int enemyStatDecreaseValue;
	int enemyStatDecreaseIndex;
	int latestFighterID;// Used to make sure that fighters all have a unique ID

	static TUniquePtr<Intermediate, TDefaultDelete<Intermediate>> instance;

	int affectedParty = -1;
	int statIndex = -1;
	float changeCrowdValue = 0;

public:

	void Victory(); //Adds 1 to the story progress //Called from the battle manager
	int GetStoryProgress();
	int GetProtagonistLevel();
	void SetProtagonistLevel(int value_);
	void UpdateCurrentRosterSize(int value_); //Value_ can be + or -. 
	int GetCurrentRosterSize();
	int GetMaxRosterSize();
	int GetMaxDeploymentSize();
	int GetLatestFighterID();
	void SetLatestFighterID(int id_);
	int GetCurrentDeploymentSize();
	int GetCurrentMoney();
	void AddFighterToSelected(FFighterTableStruct fighter_);
	void ResetSelectedFighters();
	TArray<FFighterTableStruct> GetSelectedFighters(); //Called by the battle manager
	void SpendMoney(int money_);
	void AddMoney(int money_);
	void SetNextOpponent(FOpponentStruct op_); //Called by transition to battle when the player collides with it and ends the day. 
										 //Determines the next fight when the next fight is not a story one.
	FOpponentStruct GetNextOpponent(); //Called by enemy manager to know the details of the next opponent.
	void ImprovePlayerCRD(float value_);

	TArray<int>& GetDeadUnits();
	void PushUnitToDead(int unitId_);
	void PutUnitOnHold(int index_); //Called from hubplayer or from tournament npc uictrl.

	void PlayerUnitsAreRemoved(bool remove_); //Called from central NPC when activity fails. 
											  //True means remove the units put on hold. 
											  //False means return those units to the roster and remove them from the hold array.
	static Intermediate* GetInstance();

	void ChangeStats(int partyIndex_, int statIndex_);
	int GetStatsChange(int partyIndex_);
	int GetAffecteParty();
	void ResetChangeStats(); //Make sure change stats variables are reset
};
