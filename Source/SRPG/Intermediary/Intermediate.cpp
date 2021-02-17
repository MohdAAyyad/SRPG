// Fill out your copyright notice in the Description page of Project Settings.


#include "Intermediate.h"
#include "Definitions.h"

TUniquePtr<Intermediate, TDefaultDelete<Intermediate>> Intermediate::instance = nullptr;

Intermediate::Intermediate()
{
	maxRosterSize = 20; //Arbitrary number
	currentRosterSize = 5; //Starts with 5 main characters
	protagonistLevel = 1;
	affectedParty = CHG_STAT_NON; //Affected by change stats function call. -1 neither 0 enemy 1 player
	statIndex = -1;
	changeCrowdValue = 0.0f;
	currentDeploymentSize = 0;
	latestFighterID = 0;
	bRewardsWereGiven = false;
	loadedData = FLoadData();
	bVictory = false;
}

Intermediate::~Intermediate()
{
}

Intermediate* Intermediate::GetInstance()
{
	if (!instance.Get())
		instance.Reset(new Intermediate());

	return instance.Get();
}

void Intermediate::Victory(int moneyReward_, int shardsReward_) //Adds 1 to the day counter //Called from the battle manager
{
	loadedData.currentGoldState += moneyReward_;
	loadedData.currentShardsState += shardsReward_;
	loadedData.currentDayState++;
	bHasLoadedData = true;
	bRewardsWereGiven = true;
	bVictory = true;
}

void Intermediate::Defeat(int moneyCompensation_, int shardsCompensation_)
{
	loadedData.currentGoldState += moneyCompensation_;
	loadedData.currentShardsState += shardsCompensation_;
	loadedData.currentDayState = 1;
	bHasLoadedData = true;
	bRewardsWereGiven = true;
	bVictory = false;
}


int Intermediate::GetCurrentDay()
{
	return loadedData.currentDayState; // Matches the day ID in the tables hence why it's zero-based. 1 is added to the value when displayed on the UI
}

bool Intermediate::GetRewardsWereGiven()
{
	return bRewardsWereGiven;
}

void Intermediate::ResetRewardsWereGiven()
{
	bRewardsWereGiven = false;
}

int Intermediate::GetProtagonistLevel()
{
	return protagonistLevel;
}
void Intermediate::SetProtagonistLevel(int value_)
{
}
void Intermediate::UpdateCurrentRosterSize(int value_) //Value_ can be + or -.  //Called from the fighters shop and from the battle manager
{
	currentRosterSize += value_;
}
int Intermediate::GetCurrentRosterSize()
{
	return currentRosterSize;
}
int Intermediate::GetMaxRosterSize()
{
	return maxRosterSize;
}
int Intermediate::GetCurrentDeploymentSize()
{
	return currentDeploymentSize;
}
int Intermediate::GetCurrentMoney()
{
	return loadedData.currentGoldState;
}
int Intermediate::GetCurrentShards()
{
	return loadedData.currentShardsState;
}
void Intermediate::AddFighterToSelected(FFighterTableStruct fighter_)
{
	selectedFighters.Push(fighter_);
}
TArray<FFighterTableStruct> Intermediate::GetSelectedFighters() //Called by the battle manager
{
	return selectedFighters;
}

void Intermediate::ResetSelectedFighters()
{
	if (selectedFighters.Num() > 0)
		selectedFighters.Empty(); //Needed to make sure the intermediate starts with a clean slate before every battle
}
void Intermediate::SpendMoney(int money_)
{
	if (loadedData.currentGoldState - money_ > 0)
	{
		loadedData.currentGoldState -= money_;
	}
	else
	{
		loadedData.currentGoldState = 0;
	}
}

void Intermediate::AddMoney(int money_)
{
	loadedData.currentGoldState += money_;
}

void Intermediate::SpendShards(int shards_)
{
	if (loadedData.currentShardsState - shards_ > 0)
	{
		loadedData.currentShardsState -= shards_;
	}
	else
	{
		loadedData.currentShardsState = 0;
	}
}

void Intermediate::SetNextOpponent(FOpponentStruct op_)
{
	nextOpponent = op_;
}

FOpponentStruct Intermediate::GetNextOpponent()
{
	return nextOpponent;
} 


void Intermediate::ChangeStats(int partyIndex_, int statIndex_)
{
	// Affected party determines whether the player or the enemy was affected
	// Stat index determines which stat is affected
	affectedParty = partyIndex_;
	statIndex = statIndex_;
}

int Intermediate::GetStatsChange(int partyIndex_)
{
	if (partyIndex_ == affectedParty)
	{
		return statIndex;
	}
	return -1;
}

void Intermediate::ImprovePlayerCRD(float value_)
{
	// augment the crowd value
	changeCrowdValue += value_;
}

int Intermediate::GetLatestFighterID()
{
	return latestFighterID;
}
void Intermediate::SetLatestFighterID(int id_)
{
	latestFighterID = id_;
}

TArray<int>& Intermediate::GetDeadUnits()
{
	return deadUnitsIDs;
}

void Intermediate::PushUnitToDead(int unitId_)
{
	deadUnitsIDs.Push(unitId_);
}

int Intermediate::GetAffecteParty()
{
	return affectedParty;
}

void Intermediate::ResetChangeStats()
{
	affectedParty = CHG_STAT_NON;
	statIndex = -1;
}

FLoadData& Intermediate::GetLoadedDataObject()
{
	return loadedData;
}

void Intermediate::SetLoadedDataObject(FLoadData obj_)
{
	loadedData = obj_;
}

bool Intermediate::GetHasLoadedData()
{
	return bHasLoadedData;
}
void Intermediate::SetHasLoadedData(bool value_)
{
	bHasLoadedData = value_;
}

void Intermediate::SetCurrentMoney(int money_)
{
	loadedData.currentGoldState = money_;
}

void Intermediate::SetCurrentShards(int shards_)
{
	loadedData.currentShardsState = shards_;
}

void Intermediate::SetCurrentDay(int day_)
{
	loadedData.currentDayState = day_;
}

void Intermediate::ReadyDataForSavingAfterBattle()
{
	//Remove the dead units
	if (deadUnitsIDs.Num() > 0)
	{
		for (int i = 0; i < deadUnitsIDs.Num(); i++)
		{
			for (int j = 0; j < loadedData.currentRecruitedFightersState.Num(); j++)
			{
				if (loadedData.currentRecruitedFightersState[j].id == deadUnitsIDs[i])
				{
					loadedData.currentRecruitedFightersState.RemoveAt(j);
					break;
				}
			}
		}

		deadUnitsIDs.Empty();
	}

	//Update the stats of the surviving units
	if (selectedFighters.Num() > 0)
	{
		for (int i = 0; i < selectedFighters.Num(); i++)
		{
			for (int j = 0; j < loadedData.currentRecruitedFightersState.Num(); j++)
			{
				if (loadedData.currentRecruitedFightersState[j].id == selectedFighters[i].id)
				{
					loadedData.currentRecruitedFightersState[j].hp = selectedFighters[i].hp;
					loadedData.currentRecruitedFightersState[j].pip = selectedFighters[i].pip;
					loadedData.currentRecruitedFightersState[j].atk = selectedFighters[i].atk;
					loadedData.currentRecruitedFightersState[j].def = selectedFighters[i].def;
					loadedData.currentRecruitedFightersState[j].intl = selectedFighters[i].intl;
					loadedData.currentRecruitedFightersState[j].spd = selectedFighters[i].spd;
					loadedData.currentRecruitedFightersState[j].agl = selectedFighters[i].agl;
					loadedData.currentRecruitedFightersState[j].crit = selectedFighters[i].crit;
					loadedData.currentRecruitedFightersState[j].crd = selectedFighters[i].crd;
					loadedData.currentRecruitedFightersState[j].currentEXP = selectedFighters[i].currentEXP;
					loadedData.currentRecruitedFightersState[j].neededEXPToLevelUp = selectedFighters[i].neededEXPToLevelUp;
					loadedData.currentRecruitedFightersState[j].level = selectedFighters[i].level;

					break;
				}
			}
		}
		selectedFighters.Empty();
	}
	//The items are updated by the battle manager as we need access to the table
}

bool Intermediate::GetVictory()
{
	return bVictory;
}