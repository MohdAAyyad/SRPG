// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Grid/ElementalHazard.h"
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

	class ABattlePawn* battlePawn;


	//Items
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

	//Obstacles
	UPROPERTY(EditAnywhere, Category = "Obstacles")
		class AObstaclesManager* obstacleManager;
	UPROPERTY(EditAnywhere, Category = "Obstacles")
		TArray<TSubclassOf<class ACrowdProjectile>> crowdProjectiles;
	UPROPERTY(EditAnywhere, Category = "Obstacles")
		int maxNumOfObstaclesToDestroy; //Can only destroy this number of obstacles
	UPROPERTY(EditAnywhere, Category = "Obstacles")
		int currentNumOfObstaclesDestroyed;
	UPROPERTY(EditAnywhere, Category = "Obstacles")
		int numOfTurnsToAnDestroyObstacle; //Every time this number of turns pass, the crowd will attempt to destroy an obstacle

	UPROPERTY(EditAnywhere, Category = "Audio")
		class AAudioMnager* audioMgr;
		
	int numOfTurnsPassedSinceLastObstacleWasDestroyed;
	class AObstacle* chosenObstacle;

	class ATile* neutralItemsTile;

	class AGridCharacter* champion;
	AGridCharacter* villain;

	int turnsSinceChampionAndVillainWereElected;
	bool bPermaChampion;
protected:
	void DecideOnActionForCurrentPhase();
	virtual void BeginPlay() override;
	void MoveBattlePawnOverObstacle();
	void DestroyObstacle();
	void SpawnItems();
	void ElectChampion();
	void ElectVillain();
	void EndPhase();
	int PickProjectileBasedOnHazardCurrentState(CurrentElemntalStat state_);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CalculateFavorForTheFirstTime(); //Gets the total CRD, the player's total CRD, the enemy's total CRD and calculates the player favor
	void StartCrowdPhase();
	void UpdateFavor(bool bPlayerOrVillain_); //If true, adds favorIncrement to playerFavor and subtracts it if false
	void RemoveSpawnedItem(ACrowdItem* item_);
	void AddCrowdWidgetToViewPort();	
	void SetPermaChampion(bool bvalue_);
	void ChampVillainIsDead(bool bChamp_);
	void IAmTheNewChampion(AGridCharacter* gchar_); //Called by a villain who's become the champion
	void UnElect(bool champ_);
	void FlipFavorMeter();
	void FlipFavorMeter(AGridCharacter* gchar_); //Overriden. Flip the meter against the character in the parameters
	void UpdateFavorForChamp(AGridCharacter* gchar_, int times_); //Update the favor in favor of the champ
	bool GetPermaStatus();
	void SpawnItemsAtLoc(FVector loc_);
	void BattlePawnHasFinishedMoving();


	//Reference to obstacle manager
	//Reference to battlepawn
	//Decide on what you're gonna do
	//If elemental action, ask obstacle what status effect can change it then pick a suitable projectile
	//Move the battle pawn to the location then spawn the projectile
	//Return the battlepawn to the original location

};
