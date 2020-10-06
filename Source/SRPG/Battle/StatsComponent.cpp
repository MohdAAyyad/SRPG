// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsComponent.h"
#include "Definitions.h"

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	currentStats.Reserve(26);
	tempCRD = 0;
	maxHP = 0;
	maxPip = 0;
	bLevelingUp = false;
	expOffset = 0.0f;
	addedEXP = 0;
}

void UStatsComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bLevelingUp)
	{
		if (currentStats[STAT_EXP > 0])
		{
			currentStats[STAT_EXP]--;
			addedEXP++;
			expPercentage += expOffset;
			if (addedEXP >= currentStats[STAT_NXP])
			{
				addedEXP = 0;
				expPercentage = 0.0f;
				bLevelingUp = false; //Assume we won't level up again
				CheckLevelUp(true); //Levelup

			}
		}
		else
		{
			bLevelingUp = false;
		}
	}
}

void UStatsComponent::PushAStat(int statValue_)
{
	currentStats.Push(statValue_);
}

int UStatsComponent::CalculateHit(int otherAgi_)
{
	if (FMath::RandRange(0, 20) + currentStats[STAT_WHT] >= otherAgi_) //Hit
	{
		if (FMath::RandRange(0, 100) <= currentStats[STAT_CRT]) //Crit
		{
			return ATKR_CRIT;
		}

		return ATKR_HIT;
	}
	return ATKR_MISS;
}
void UStatsComponent::TakeDamageToStat(int stat_, int value_)
{
	if (stat_ >= 0 && stat_ < currentStats.Num())
	{
		switch (stat_)
		{
		case STAT_HP: //Lose health
			currentStats[STAT_HP] -= value_;
			if (currentStats[STAT_HP] <= 0)
			{
				//TODO
				//Handle Death
			}
			break;
		default: //Handle debuffs
			currentStats[stat_] -= value_;
			if (currentStats[stat_] < 0)
				currentStats[stat_] = 0;
			break;
		}
	}
}
void UStatsComponent::AddToStat(int stat_, int value_)
{
	if (stat_ >= 0 && stat_ < currentStats.Num())
	{
		switch (stat_)
		{
		case STAT_HP: //Regain health
			currentStats[STAT_HP] += value_;
			if (currentStats[STAT_HP] > maxHP)
			{
				currentStats[STAT_HP] = maxHP;
			}	
			else if (currentStats[STAT_HP] < 0)
			{
				currentStats[STAT_HP] = 0;
			}
			break;
		case STAT_PIP: //Regain pips
			currentStats[STAT_PIP] += value_;
			if (currentStats[STAT_PIP] > maxPip)
			{
				currentStats[STAT_PIP] = maxPip;
			}
			else if (currentStats[STAT_PIP] < 0)
			{
				currentStats[STAT_PIP] = 0;
			}
			break;
		default: //Handle buffs
			currentStats[stat_] += value_;
			break;
		}
	}
}
bool UStatsComponent::AddTempCRD(int value_) //True if the CRD stat is increased
{
	tempCRD += value_;
	if (tempCRD >= CRD_THRESHOLD)
	{
		tempCRD -= CRD_THRESHOLD;
		currentStats[STAT_CRD] += 1;

		return true;
	}
	return false;
}
void UStatsComponent::CheckLevelUp(bool hasLeveledUp_)
{
	if (hasLeveledUp_)
	{
		//Levelup
		currentStats[STAT_NXP] *= 2;

		//TODO
		//Update the stats

		if (currentStats[STAT_EXP] > 0) //We still have exp to earn
			CheckLevelUp(false); //Call the function again to calculate the expOffset again
	}
	else
	{
		expOffset = 1.0f / currentStats[STAT_NXP]; //The point here is to try to minimize the amount of divisions made.
		bLevelingUp = true;		
	}
}
int UStatsComponent::GetStatValue(int stat_)
{
	if (stat_ >= 0 && stat_ < currentStats.Num())
	{
		return currentStats[stat_];
	}
	else if (stat_ == -1)
	{
		return maxHP;
	}
	else if (stat_ == -2)
	{
		return maxPip;
	}

	return 0;
}
void UStatsComponent::ScaleLevelWithArchetype(int targetLevel_, int archetype_)
{
	//TODO

	//Binary increase based on archetype
	// Non-archetype stats have 1 added every two levels while the archetype stat gets a 1 added for every level
	int currentLevel = currentStats[STAT_LVL];
	int increment = currentLevel % 2;
	while (currentLevel < targetLevel_)
	{
		increment = currentLevel % 2;
		currentStats[STAT_HP] += increment;
		currentStats[STAT_PIP] += increment;
		switch (archetype_)
		{
		case ARCH_ATK:
			currentStats[STAT_ATK]++;
			for (int i = STAT_ATK + 1; i < STAT_LVL; i++)
				currentStats[i] += increment;
			break;
		case ARCH_DEF:
			currentStats[STAT_ATK]+=increment;
			currentStats[STAT_DEF]++;
			for (int i = STAT_DEF + 1; i < STAT_LVL; i++)
				currentStats[i] += increment;
			break;
		case ARCH_INT:
			currentStats[STAT_ATK] += increment;
			currentStats[STAT_DEF] += increment;
			currentStats[STAT_INT]++;
			for (int i = STAT_INT + 1; i < STAT_LVL; i++)
				currentStats[i] += increment;
			break;
		default:
			break;
		}

		currentLevel++;
	}
	currentStats[STAT_LVL] = targetLevel_;
	currentStats[STAT_ARCH] = archetype_;
}

void UStatsComponent::InitStatsAtZero()
{
	for (int i = 0; i < 26; i++)
		currentStats.Push(0);

	currentStats[STAT_LVL] = 1; //Min level
	currentStats[STAT_SPD] = 2; //Min speed
}


void UStatsComponent::UpdateMaxHpAndMaxPip(int hp_, int pip_)
{
	//Called when adding weapon stats which happens after updating the currentStats array
	maxHP = currentStats[STAT_HP] + hp_;
	maxPip = currentStats[STAT_PIP] + pip_;
}


float UStatsComponent::GetHPPercentage()
{
	return (static_cast<float>(currentStats[STAT_HP]) / static_cast<float>(maxHP));
}

float UStatsComponent::GetPIPPercentage()
{
	return (static_cast<float>(currentStats[STAT_PIP]) / static_cast<float>(maxPip));
}

float UStatsComponent::GetEXPPercentage()
{
	return expPercentage;
}


void UStatsComponent::UpdateChampionVillainStats(bool champion_)
{
	if (champion_)
	{
		currentStats[STAT_HP] = maxHP;
		currentStats[STAT_ATK] += CHAMP_ATK;
		currentStats[STAT_DEF] += CHAMP_DEF;
		currentStats[STAT_INT] += CHAMP_INT;
		currentStats[STAT_SPD] += CHAMP_SDP;
		AddToStat(STAT_PIP,CHAMP_PIP);
	}
	else
	{
		currentStats[STAT_ATK] += VILL_ATK;
		currentStats[STAT_DEF] += VILL_DEF;
		currentStats[STAT_INT] += VILL_INT;
		currentStats[STAT_SPD] += VILL_SPD;
		AddToStat(STAT_PIP, VILL_PIP);
	}
}
void UStatsComponent::RevertChampionVillainStatsUpdate(bool champion_)
{
	if (champion_)
	{
		currentStats[STAT_ATK] -= CHAMP_ATK;
		currentStats[STAT_DEF] -= CHAMP_DEF;
		currentStats[STAT_INT] -= CHAMP_INT;
		currentStats[STAT_SPD] -= CHAMP_SDP;
	}
	else
	{
		currentStats[STAT_ATK] -= VILL_ATK;
		currentStats[STAT_DEF] -= VILL_DEF;
		currentStats[STAT_INT] -= VILL_INT;
		currentStats[STAT_SPD] -= VILL_SPD;
	}
}