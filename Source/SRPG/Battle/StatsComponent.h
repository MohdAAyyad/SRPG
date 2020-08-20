// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SRPG_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStatsComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<int> currentStats;
	int tempCRD;
	int wpnSkillsIndex;
	int wpnRowSpeed;
	int wpnDepthSpeed;
	int armSkillsIndex;
	UPROPERTY(BlueprintReadOnly)
		int maxHP;
	UPROPERTY(BlueprintReadOnly)
		int maxPip;
	int archetype;

public:	
	void UpdateStats(TArray<int>& currentStats);
	int CalculateHit(int otherAgi_);
	void TakeDamageToStat(int stat_, int value_);
	void AddToStat(int stat_, int value_);
	bool AddTempCRD(int value_); //True if the CRD stat is increased
	void LevelUp();
	void ScaleLevelWithArchetype(int targetLevel_, int archetype_); //Mainly used by enemies
	int GetStatValue(int stat_);
	void UpdateWpnArmSkillsIndexes(int wpn_, int wpnR_, int wpnD_, int wpnHit_,int arm_);
	void InitStatsAtZero(); //Mainly used by enemies. Followed by a call to scalelevelwitharchetype
};
