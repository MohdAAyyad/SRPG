// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsComponent.h"
#include "Definitions.h"

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	currentStats.Reserve(19);
	tempCRD = 0;
	wpnSkillsIndex = 0;
	wpnRowSpeed = 0;
	wpnDepthSpeed = 0;
	armSkillsIndex = 0;
	maxHP = 0;
	maxPip = 0;
	archetype = 0;

	// ...
}

void UStatsComponent::UpdateStats(TArray<int>& currentStats_)
{
	currentStats = currentStats_;
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

void UStatsComponent::UpdateWpnArmSkillsIndexes(int wpn_, int wpnR_, int wpnD_, int wpnHit_, int arm_)
{
	wpnSkillsIndex = wpn_;
	wpnRowSpeed = wpnR_;
	wpnDepthSpeed = wpnD_;
	armSkillsIndex = arm_;
	currentStats.Push(wpnHit_); //Should be index 18
}

void UStatsComponent::ScaleLevelWithArchetype(int targetLevel_, int archetype_)
{
	//TODO
}

void UStatsComponent::InitStatsAtZero()
{
	for (int i = 0; i < 19; i++)
		currentStats.Push(0);
}

