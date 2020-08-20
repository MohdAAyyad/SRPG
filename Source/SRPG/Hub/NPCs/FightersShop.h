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
	void LoadText() override;
	TArray<FFighter> GetAllFightersForSale();

	FString fighterDisplayValue;
	// array of all the names in the table
	TArray<FName> rowNames;
	// the current index that was selected 
	int currentIndex;
	//makes sure we've chosen a fighter before allowing anything to be pushed
	bool haveChosenFighter;

	bool hasLeveledUp;
	TArray<int> statsAfterLevelUp;

	UPROPERTY(EditAnywhere)
	class AHubWorldManager* hub;

public:
	UFUNCTION(BlueprintCallable)
	void ChooseFighter(int fighterIndex_);
	UFUNCTION(BlueprintCallable)
	void UpdateName(FString name_);
	UFUNCTION(BlueprintCallable)
	FString GetFighterInfo(int fighterIndex_);
	UFUNCTION(BlueprintCallable)
	TArray<int> LevelUpFighter();

	UFUNCTION(BlueprintCallable)
		FString PrintFighter1();
	UFUNCTION(BlueprintCallable)
		FString PrintFighter2();
	UFUNCTION(BlueprintCallable)
		FString PrintFighter3();
	UFUNCTION(BlueprintCallable)
	TArray<int> LevelDownFighter();

	UFUNCTION(BlueprintCallable)
	void FinalizePurchase();

	//how many fighters have we purchased
	UPROPERTY(EditAnywhere)
	int purchasedFighters;

	int fighterID; // Increased by 1 for every fighter. Used to make sure no two fighters are the same

	UFUNCTION(BlueprintCallable)
	void CalculatePrice();



};
