// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"

UCLASS()
class SRPG_API AObstacle : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AObstacle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Obstacle")
		USceneComponent* root;

	UPROPERTY(EditAnywhere, Category = "Destructible Mesh")
		class UDestructibleComponent* mesh;

	UPROPERTY(EditAnywhere, Category = "Destructible Mesh")
		float hp;

	UPROPERTY(EditAnywhere, Category = "Obstacle")
		class UBoxComponent* box;

	UPROPERTY(EditAnywhere, Category = "Persistence")
		bool bCanBeRemoved; //If true, the obstacle manager will have the chance to destroy this obstacle at begin play

	UPROPERTY(EditAnywhere, Category = "ObstacleManager")
		class AObstaclesManager* obstacleManager;

	TArray<class ATile*> obstructedTiles;

	void AddObstacleToObstacleManager();

	//TODO
	//Update obstacle take damage to take a status effect
	//Override the function in elementals
	//Finish up the elemental code
	//Update the grid character take damage functions to take into account status effects
	//Update the stats component to make use of the status effects



public:
	virtual void ObstacleTakeDamage(float damage_, int statusEffect_);
	virtual bool AddObstructedTile(ATile* tile_); //Returns true if the tiles are blocked and false otherwise.
	bool IsAnyOfMyTilesHighlighted(int highlightIndex_);

	TArray<ATile*> GetObstructedTiles();
	virtual void DelayedDestroy();

	virtual void ATurnHasPassed(int turnType_) {};//Overriden in elementals


};
