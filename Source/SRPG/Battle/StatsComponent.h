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
	UPROPERTY(BlueprintReadOnly)
		int maxHP;
	UPROPERTY(BlueprintReadOnly)
		int maxPip;

	class AGridCharacter* ownerChar;

	float expOffset; // 1.0f/nxp //Used to avoid having to divide every tick
	float expPercentage;
	int addedEXP;
	bool bLevelingUp;
	int newLevel;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	TArray<int> turnsSinceLastStatChange;
	TArray<int> tempStatChange;
	TArray<int> activeStatusEffects;
	TArray<int> turnsSinceStatusEffect;


public:	
	int CalculateHit(int otherAgi_);
	void TakeDamageToStat(int stat_, int value_);
	void AddToStat(int stat_, int value_);
	bool AddTempCRD(int value_); //True if the CRD stat is increased
	void CheckLevelUp(bool hasLeveledUp_);
	void ScaleLevelWithArchetype(int targetLevel_, int archetype_); //Mainly used by enemies
	int GetStatValue(int stat_);
	void InitStatsAtZero(); //Mainly used by enemies. Followed by a call to scalelevelwitharchetype
	void PushAStat(int statValue_);
	void UpdateMaxHpAndMaxPip(int hp_, int pip_);

	UFUNCTION(BlueprintCallable)
		float GetHPPercentage();

	UFUNCTION(BlueprintCallable)
		float GetPIPPercentage();

	UFUNCTION(BlueprintCallable)
		float GetEXPPercentage();

	void UpdateChampionVillainStats(bool champion_);
	void RevertChampionVillainStatsUpdate(bool champion_); //When three turns pass and no perma champion is on the field, the stats will need to be reverted
	void SetOwnerRef(AGridCharacter* ref_);

	int GetMaxHP();
	int GetMaxPIP();
	void CheckStatBuffNerfStatus(); //Checks whether buffs and nerfs should be negated
	bool HasThisStatBeenBuffed(int statIndex_);
	void AddTempToStat(int statIndeX_, int value_); //Handle buffs nerfs
	void CheckIfAffectedByStatusEffect(int effect_);
protected:
	void FinishLevlingUp();
	int ConvertStatIndexToTempStatIndex(int statIndex_);
	int ConvertTempStatIndexToStatIndex(int tempStatIndex_);
};
