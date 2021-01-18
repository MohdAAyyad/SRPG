// Fill out your copyright notice in the Description page of Project Settings.


#include "Intermediate.h"
#include "Definitions.h"

TUniquePtr<Intermediate, TDefaultDelete<Intermediate>> Intermediate::instance = nullptr;

Intermediate::Intermediate()
{
	maxRosterSize = 20; //Arbitrary number
	currentRosterSize = 5; //Starts with 5 main characters
	dayCounter = 0; //Matches the day ID in the tables hence why it's zero. 1 is added to the value when displayed on the UI
	protagonistLevel = 1;
	currentMoney = 50000;
	currentShards = 25;
	affectedParty = CHG_STAT_NON; //Affected by change stats function call. -1 neither 0 enemy 1 player
	statIndex = -1;
	changeCrowdValue = 0.0f;
	maxDeploymentSize = 10;
	currentDeploymentSize = 0;
	latestFighterID = 4;
	bRewardsWereGiven = false;
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
	currentMoney += moneyReward_;
	currentShards += shardsReward_;
	dayCounter++;
	bRewardsWereGiven = true;
}

void Intermediate::Defeat(int moneyCompensation_, int shardsCompensation_)
{
	currentMoney += moneyCompensation_;
	currentShards += shardsCompensation_;
	dayCounter = 0;
	bRewardsWereGiven = true;
}
int Intermediate::GetCurrentDay()
{
	return dayCounter + 1; // Matches the day ID in the tables hence why it's zero-based. 1 is added to the value when displayed on the UI
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
int Intermediate::GetMaxDeploymentSize()
{
	return maxDeploymentSize;
}
int Intermediate::GetCurrentDeploymentSize()
{
	return currentDeploymentSize;
}
int Intermediate::GetCurrentMoney()
{
	return currentMoney;
}
int Intermediate::GetCurrentShards()
{
	return currentShards;
}
void Intermediate::AddFighterToSelected(FFighterTableStruct fighter_)
{
	if (currentDeploymentSize < maxDeploymentSize)
	{
		selectedFighters.Push(fighter_);
	}
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
	if (currentMoney - money_ > 0)
	{
		currentMoney -= money_;
	}
	else
	{
		currentMoney = 0;
	}
}

void Intermediate::SpendShards(int shards_)
{
	if (currentShards - shards_ > 0)
	{
		currentShards -= shards_;
	}
	else
	{
		currentShards = 0;
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
	if (unitId_ > 4) //>4 as we have 5 main characters. All of which are immune to permadeath
	{
		deadUnitsIDs.Push(unitId_);
	}
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