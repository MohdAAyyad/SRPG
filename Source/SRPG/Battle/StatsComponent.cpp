// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsComponent.h"
#include "Definitions.h"

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	currentStats.Reserve(26);
	tempCRD = 0;
	maxHP = 0;
	maxPip = 0;
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
			break;
		case STAT_PIP: //Regain pips
			currentStats[STAT_PIP] += value_;
			if (currentStats[STAT_PIP] > maxPip)
			{
				currentStats[STAT_PIP] = maxPip;
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
	if (tempCRD >= 100)
	{
		tempCRD -= 100;
		currentStats[STAT_CRD] += 1;

		return true;
	}
	return false;
}
void UStatsComponent::LevelUp()
{
	//TODO
	//Level up the stats based on the archetype
}
int UStatsComponent::GetStatValue(int stat_)
{
	if (stat_ >= 0 && stat_ < currentStats.Num())
		return currentStats[stat_];

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

	//UE_LOG(LogTemp, Warning, TEXT("Speed after scaling %d"), currentStats[STAT_SPD]);
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

