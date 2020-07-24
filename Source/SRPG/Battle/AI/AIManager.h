// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIManager.generated.h"

UCLASS()
class SRPG_API AAIManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Battle")
		class ABattleManager* btlManager;
	UPROPERTY(EditAnywhere, Category = "Battle")
		class AGridManager* gridManager;

	UPROPERTY(EditAnywhere, Category = "Enemies")
		TArray<TSubclassOf<class AEnemyBaseGridCharacter>> enemiesBPs;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		int enemyRowDeploymentIndex;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		int enemyDeploymentOffset;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		int enemyDeploymentRowSpeed;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		int enemyDeploymentDepth;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		int thisIntShouldBeReplacedWithTheSOpponentNumberOfTroopsVariable;

	TArray<class ATile*> enemyDeploymentTiles;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BeginEnemyTurn(); //Called from Battlemanager

};
