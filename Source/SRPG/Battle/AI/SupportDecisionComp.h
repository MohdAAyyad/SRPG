// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/AI/DecisionComp.h"
#include "SupportDecisionComp.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API USupportDecisionComp : public UDecisionComp
{
	GENERATED_BODY()
public:
	USupportDecisionComp();
	AGridCharacter* FindTheOptimalTargetCharacter() override;
	class ATile* FindOptimalTargetTile(ATile* myTile_) override;
protected:
	AGridCharacter* FindHealTarget(AEnemyBaseGridCharacter* ignoreThisTarget);
	AGridCharacter* FindBuffTarget(TArray<AEnemyBaseGridCharacter*> echars, FVector myLoc);
	AGridCharacter* FindClosestAllyWithTheLeastHealth(TArray<AEnemyBaseGridCharacter*> echars, FVector myLoc);
};
