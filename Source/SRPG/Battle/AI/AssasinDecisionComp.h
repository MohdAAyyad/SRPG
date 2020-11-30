// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/AI/DecisionComp.h"
#include "AssasinDecisionComp.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API UAssasinDecisionComp : public UDecisionComp
{
	GENERATED_BODY()
public:
	UAssasinDecisionComp();
	AGridCharacter* FindTheOptimalTargetCharacter() override;
	class ATile* FindOptimalTargetTile(ATile* myTile_) override;
protected:
	AGridCharacter* FindTargetWithLeastHealth(class APlayerGridCharacter* ignoreThis);
	AGridCharacter* FindClosestTarget(TArray<APlayerGridCharacter*> pchars, FVector myLoc);
	
};
