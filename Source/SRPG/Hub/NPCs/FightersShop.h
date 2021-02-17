// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hub/NPC.h"
#include "ExternalFileReader/FighterTableStruct.h"
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
	AFightersShop();
	UPROPERTY(BlueprintReadWrite)
		FFighterTableStruct chosenFighter;
	void BeginPlay() override;
	TArray<FFighterTableStruct> CalculatedStats;
	//
	UFUNCTION(BlueprintCallable)
	TArray<FFighterTableStruct> GetAllFightersForSale();


	TArray<int> statsAfterLevelUp;


	//Displayed fighters
	class AFighterShopDisplayedFighter* displayedFighter;

	UPROPERTY(EditAnywhere, Category = "DisplayedFighter")
		FVector displayLocationOffset;
	UPROPERTY(EditAnywhere, Category = "DisplayedFighter")
		FVector displayScale;
	UPROPERTY(EditAnywhere, Category = "DisplayedFighter")
		TArray<TSubclassOf<AFighterShopDisplayedFighter>> fightersToDisplay;

	UFUNCTION(BlueprintCallable)
		void SaveNewRecruitedFighters(); //Called from the UI when you leave the shop after buying one or more fighters

	void LoadRecruitedFighters();
public:
	UFUNCTION(BlueprintCallable)
		void SelectFighterToBuy(FFighterTableStruct fighter_); //Called by the UI
	UFUNCTION(BlueprintCallable)
		void LevelUpFighter(int value_); //Value_ could be + or -
	UFUNCTION(BlueprintCallable)
		int GetCurrentPrice();

	UFUNCTION(BlueprintCallable)
		void UpdateChosenFighterName(FString name_);

	UFUNCTION(BlueprintCallable)
	void FinalizePurchase();

	int fighterID; // Increased by 1 for every fighter. Used to make sure no two fighters are the same

	UFUNCTION(BlueprintCallable)
		void DisplayFighter(int index_);

	UFUNCTION(BlueprintCallable)
		void RemoveDisplayedFighter();


	void LeaveNPC() override;


	void OnOverlapWithPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult) override;


};
