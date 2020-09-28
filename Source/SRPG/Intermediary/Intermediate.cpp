// Fill out your copyright notice in the Description page of Project Settings.


#include "Intermediate.h"
#include "Definitions.h"

TUniquePtr<Intermediate, TDefaultDelete<Intermediate>> Intermediate::instance = nullptr;

Intermediate::Intermediate()
{
	maxRosterSize = 20; //Arbitrary number
	currentRosterSize = 5; //Starts with 5 main characters
	storyProgress = 0;
	protagonistLevel = 1;
	currentMoney = 50000;
	affectedParty = CHG_STAT_NON; //Affected by change stats function call. -1 neither 0 enemy 1 player
	statIndex = -1;
	changeCrowdValue = 0.0f;
	maxDeploymentSize = 10;
	currentDeploymentSize = 0;
	latestFighterID = 4;
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

void Intermediate::Victory() //Adds 1 to the story progress //Called from the battle manager
{
}
int Intermediate::GetStoryProgress()
{
	return storyProgress;
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
void Intermediate::AddFighterToSelected(FFighterTableStruct fighter_)
{
	if (currentDeploymentSize < maxDeploymentSize)
	{
		selectedFighters.Push(fighter_);
	}
}

void Intermediate::PutUnitOnHold(int index_)
{

}

void Intermediate::PlayerUnitsAreRemoved(bool remove_)
{

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
void Intermediate::AddMoney(int money_)
{
	// if a negative number was input make it positive
	if (money_ < 0)
	{
		money_ *= -1;
	}

	currentMoney += money_;

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