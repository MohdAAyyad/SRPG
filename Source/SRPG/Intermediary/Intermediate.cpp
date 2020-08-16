// Fill out your copyright notice in the Description page of Project Settings.


#include "Intermediate.h"

TUniquePtr<Intermediate, TDefaultDelete<Intermediate>> Intermediate::instance = nullptr;

Intermediate::Intermediate()
{
	maxRosterSize = 20; //Arbitrary number
	currentRosterSize = 4; //Starts with 4 main characters
	storyProgress = 0;
	protagonistLevel = 1;
	currentMoney = 1000;
	enemyStatDecreaseValue = 0;
	enemyStatDecreaseIndex = 0;
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
void Intermediate::UpdateCurrentRosterSize(int value_) //Value_ can be + or -. 
{
}
int Intermediate::GetCurrentRosterSize()
{
	return currentRosterSize;
}
int Intermediate::GetMaxRosterSize()
{
	return maxRosterSize;
}
int Intermediate::GetCurrentMoney()
{
	return currentMoney;
}
void Intermediate::AddFighterToSelected(int index_)
{
}
TArray<int> Intermediate::GetSelectedFighters() //Called by the battle manager
{
	return selectedFighters;
}
void Intermediate::SpendMoney(int money_)
{
}
void Intermediate::SetNextOpponent(FOpponentStruct op_)
{
	nextOpponent = op_;
}
FOpponentStruct Intermediate::GetNextOpponent()
{
	return nextOpponent;
}
void Intermediate::PlayerStatsGoUp(int value_, int statIndex_) //Called by central NPC when an activity succeeds 
															 //and increases roster stats
{
} 
void Intermediate::EnemyStatsGoDown(int value_, int statIndex_)//Passed to battle manager later on
{
} 
void Intermediate::PutUnitOnHold(int index_)//Called from hubplayer or from tournament npc uictrl.
{
} 
void Intermediate::PlayerUnitsAreRemoved(bool remove_)//Called from central NPC when activity fails. 
													  //True: remove the units put on hold. 
													 //False: return those units to the roster and remove them from the hold array.
{
} 
void Intermediate::ImprovePlayerCRD(float value_)
{
}