// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hub/NPC.h"
#include "Hub/NPCs/Fighter.h"
#include "FightersShop.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API AFightersShop : public ANPC
{
	GENERATED_BODY()
protected:
	// which fighter is selected in the UI
	FFighter chosenFighter;
	void BeginPlay() override;
	/*When the player decides to level up we save a copy of the SFighter 
	so that when the player fluctuates between the levels the results no longer follow a random chance 
	and are consistent i.e. this is used to make sure the random chance is only run once on level up.*/
	TArray<FFighter> CalculatedStats;
	//
	int calculatedStatsIterator;
	void EndDialogue() override;

	UFUNCTION(BlueprintCallable)
	TArray<FFighter> GetAllFightersForSale();



public:
	void ChooseFighter(int fighterIndex_);
	void UpdateName(FString name_);

	UFUNCTION(BlueprintCallable)
	TArray<int> LevelUpFighter();

	UFUNCTION(BlueprintCallable)
	TArray<int> LevelDownFighter();

	UFUNCTION(BlueprintCallable)
	void FinalizePurchase();

	//how many fighters have we purchased
	UPROPERTY(EditAnywhere)
	int purchasedFighters;



};
