// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleCrowd.generated.h"

UCLASS()
class SRPG_API ABattleCrowd : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABattleCrowd();

protected:
	UPROPERTY(EditAnywhere, Category = "Root")
		USceneComponent* root;
	UPROPERTY(EditAnywhere, Category = "BattleManager")
		class ABattleManager* btlManager;
	UPROPERTY(EditAnywhere, Category = "AI")
		class AAIManager* aiManager;
	UPROPERTY(EditAnywhere, Category = "Grid")
		class AGridManager* gridManager;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Favor")
		float playerFavor; //How much the crowd favors the player. Enemy favor is 100-playerFavor
	UPROPERTY(EditAnywhere, Category = "Favor")
		float favorIncrement; //How much favor increases for every CRD point
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
		class UWidgetComponent* widgetComp;


	UPROPERTY(EditAnywhere, Category = "Items")
		TArray<TSubclassOf<class ACrowdItem>> crowdItems;
	TArray<ACrowdItem*> spawnedItems;
	UPROPERTY(EditAnywhere, Category = "Items")
		int rowIndexForNeutralItems; //Where to spawn items if the crowd is still neutral
	UPROPERTY(EditAnywhere, Category = "Items")
		int offsetForNeutralItems;
	UPROPERTY(EditAnywhere, Category = "Items")
		int rowSpeedForSpawningItems;
	UPROPERTY(EditAnywhere, Category = "Items")
		int depthForSpawningItems;
	UPROPERTY(EditAnywhere, Category = "Items")
		int numOfItemsSpawnedPerTurn; //Increases by 1 every numOfTurnsToPassBeforeItemsSpawnedIsIncreased turns
	UPROPERTY(EditAnywhere, Category = "Items")
		int numOfTurnsToPassBeforeItemsSpawnedIsIncreased;
	UPROPERTY(EditAnywhere, Category = "Items")
		int maxNumOfItemsSpawnedPerTurn;

	class ATile* neutralItemsTile;

	class AGridCharacter* champion;
	AGridCharacter* villain;

	int turnsSinceChampionAndVillainWereElected;
	bool bPermaChampion;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SpawnItems();
	void ElectChampionAndVillain();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CalculateFavorForTheFirstTime(); //Gets the total CRD, the player's total CRD, the enemy's total CRD and calculates the player favor
	void StartCrowdPhase();
	void UpdateFavor(bool bPlayerOrVillain_); //If true, adds favorIncrement to playerFavor and subtracts it if false
	void RemoveSpawnedItem(ACrowdItem* item_);
	void AddCrowdWidgetToViewPort();	

};
