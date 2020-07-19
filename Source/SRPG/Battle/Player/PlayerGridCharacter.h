// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/GridCharacter.h"
#include "PlayerGridCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API APlayerGridCharacter : public AGridCharacter
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable)
		void HighlightMovementPath();

	UFUNCTION(BlueprintCallable)
		void HighlightRegularAttackPath();
public:
	void Selected() override;
	void NotSelected() override;
	
};
