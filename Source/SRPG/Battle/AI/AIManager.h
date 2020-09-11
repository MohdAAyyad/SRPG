// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExternalFileReader/FOpponentStruct.h"
#include "AIManager.generated.h"

USTRUCT(BlueprintType)
struct SRPG_API FAIDeploymentStruct
{
	GENERATED_USTRUCT_BODY()

	FAIDeploymentStruct()
	{
		enemyRowDeploymentIndex =
			enemyDeploymentOffset =
			enemyDeploymentRowSpeed =
			enemyDeploymentDepth =
			maxNumOfEnemiesForThisNode =
			currentNumOfEnemiesForThisNode = 0;
	}

	UPROPERTY(EditAnywhere, Category = "Enemies")
		int enemyRowDeploymentIndex;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		int enemyDeploymentOffset;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		int enemyDeploymentRowSpeed;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		int enemyDeploymentDepth;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		int maxNumOfEnemiesForThisNode;

		int currentNumOfEnemiesForThisNode;
};

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

		class ABattleManager* btlManager;
		class AGridManager* gridManager;

	UPROPERTY(EditAnywhere, Category = "Enemies")
		TArray<TSubclassOf<class AEnemyBaseGridCharacter>> enemiesBPs;
	UPROPERTY(EditAnywhere, Category = "Enemies")
		TArray<FAIDeploymentStruct> depNodes;

	TArray<AEnemyBaseGridCharacter*> deployedEnemies;
	int numberOfEnemiesWhichFinishedMoving; //When this number reaches the current number of troops, tell them to attack
	int numberOfEnemiesToldToMove;
	FOpponentStruct nextOp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BeginEnemyTurn(); //Called from Battlemanager
	void SetBattleAndGridManager(ABattleManager* btl_, AGridManager* grid_);
	void FinishedMoving();
	void FinishedAttacking();
	void OrderEnemiesToAttackPlayer();

	float GetTotalStatFromDeployedEnemies(int statIndex_);

	class AGridCharacter* GetEnemyWithHighestStat(int statIndex_);
	AGridCharacter* GetEnemyWithLowestStat(int statIndex_);

protected:

	void TellDeployedEnemiesToMove();

};
