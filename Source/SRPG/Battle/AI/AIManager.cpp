// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"
#include "../BattleManager.h"
#include "../Grid/GridManager.h"
#include "../Grid/Tile.h"
#include "Engine/World.h"
#include "EnemyBaseGridCharacter.h"


// Sets default values
AAIManager::AAIManager()
{
	PrimaryActorTick.bCanEverTick = false;
	numberOfEnemiesWhichFinishedMoving = 0;

}

// Called when the game starts or when spawned
void AAIManager::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIManager::BeginEnemyTurn()
{
	for (int i = 0; i < deployedEnemies.Num(); i++)
	{
		deployedEnemies[i]->EnableDetectionCollision();
	}

	for (int i = 0; i < deployedEnemies.Num(); i++)
	{
		deployedEnemies[i]->SetMoving(true);
	}
}

void AAIManager::SetBattleAndGridManager(ABattleManager* btl_, AGridManager* grid_)
{
	//Called by the battle manager at begin play
	btlManager = btl_;
	gridManager = grid_;
	TArray<ATile*> enemyDeploymentTiles;

	if (gridManager)
	{
		gridManager->HighlightDeploymentTiles(enemyRowDeploymentIndex, enemyDeploymentOffset,
			enemyDeploymentRowSpeed, enemyDeploymentDepth);

		enemyDeploymentTiles = gridManager->GetHighlightedTiles();

		for (int i = 0; i < thisIntShouldBeReplacedWithTheSOpponentNumberOfTroopsVariable; i++)
		{
			int enemyIndex = FMath::RandRange(0, enemiesBPs.Num() - 1);
			int tileIndex = FMath::RandRange(0, enemyDeploymentTiles.Num() - 1);
			FVector loc = enemyDeploymentTiles[tileIndex]->GetActorLocation();
			loc.Z += 50.0f;
			AEnemyBaseGridCharacter* enemy = GetWorld()->SpawnActor<AEnemyBaseGridCharacter>(enemiesBPs[enemyIndex], loc, FRotator(0.0f, 180.0f, 0.0f));

			if (enemy)
			{
				enemy->SetManagers(this, gridManager);
				deployedEnemies.Push(enemy);
			}
		}
		gridManager->ClearHighlighted();
	}
}

void AAIManager::FinishedMoving()
{
	numberOfEnemiesWhichFinishedMoving++;

	if (numberOfEnemiesWhichFinishedMoving == thisIntShouldBeReplacedWithTheSOpponentNumberOfTroopsVariable)
		OrderEnemiesToAttackPlayer();
}
void AAIManager::FinishedAttacking()
{
	numberOfEnemiesWhichFinishedMoving--;
	if (numberOfEnemiesWhichFinishedMoving >= 0)
		OrderEnemiesToAttackPlayer();
}

void AAIManager::OrderEnemiesToAttackPlayer()
{
	if (numberOfEnemiesWhichFinishedMoving - 1 < deployedEnemies.Num() && numberOfEnemiesWhichFinishedMoving - 1 >= 0)
	{
		if (deployedEnemies[numberOfEnemiesWhichFinishedMoving - 1])
		{
			deployedEnemies[numberOfEnemiesWhichFinishedMoving - 1]->ExecuteChosenAttack();
		}
	}
}

