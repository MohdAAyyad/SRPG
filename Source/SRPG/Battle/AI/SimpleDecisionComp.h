// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/AI/DecisionComp.h"
#include "SimpleDecisionComp.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API USimpleDecisionComp : public UDecisionComp
{
	GENERATED_BODY()
public:
	USimpleDecisionComp();
	AGridCharacter* FindTheOptimalTargetCharacter() override;
	AGridCharacter* FindClosestPlayerTarget(class APlayerGridCharacter* ignoreTihs);
	class ATile* FindOptimalTargetTile(ATile* myTile_) override;

};
