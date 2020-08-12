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
	numberOfEnemiesWhichFinishedMoving = -1;

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
	numberOfEnemiesWhichFinishedMoving = -1;
	for (int i = 0; i < deployedEnemies.Num(); i++)
	{
		deployedEnemies[i]->StartEnemyTurn();
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

		for (int i = 0; i < thisIntShouldBeReplacedWithTheSOpponentNumberOfTroopsVariable;)
		{
			int enemyIndex = FMath::RandRange(0, enemiesBPs.Num() - 1);
			int tileIndex = FMath::RandRange(0, enemyDeploymentTiles.Num() - 1);
			
			if (enemyDeploymentTiles[tileIndex]->GetOccupied()) //Make sure the tile was never selected before to avoid two enemies being spawned on the same tile
				continue;


			FVector loc = enemyDeploymentTiles[tileIndex]->GetActorLocation();
			loc.Z += 50.0f;
			AEnemyBaseGridCharacter* enemy = GetWorld()->SpawnActor<AEnemyBaseGridCharacter>(enemiesBPs[enemyIndex], loc, FRotator(0.0f, 180.0f, 0.0f));

			if (enemy)
			{
				enemy->SetManagers(this, gridManager,btlManager);
				deployedEnemies.Push(enemy);
				enemyDeploymentTiles[tileIndex]->SetOccupied(true);
				i++;

				//TODO
				//Use the information obtained from the SOppOnent struct coming from the intermediate to update the stats of the enemies

			}
		}
		gridManager->ClearHighlighted();
	}
}

void AAIManager::FinishedMoving()
{

	//Called by enemies when they finish moving. Used to know when to tell the enemies to attack
	numberOfEnemiesWhichFinishedMoving++;
	UE_LOG(LogTemp, Warning, TEXT("Called AI Manager finished moving %d , %d"), numberOfEnemiesWhichFinishedMoving, thisIntShouldBeReplacedWithTheSOpponentNumberOfTroopsVariable - 1);
	if (numberOfEnemiesWhichFinishedMoving == (thisIntShouldBeReplacedWithTheSOpponentNumberOfTroopsVariable - 1))
		OrderEnemiesToAttackPlayer();
}
void AAIManager::FinishedAttacking()
{
	//When an enemy finishes attacking, this function is called so we can tell the next one to attack.
	numberOfEnemiesWhichFinishedMoving--;
	if (numberOfEnemiesWhichFinishedMoving >= 0 && numberOfEnemiesWhichFinishedMoving < deployedEnemies.Num())
		OrderEnemiesToAttackPlayer();
}

void AAIManager::OrderEnemiesToAttackPlayer()
{
	//Attacking happens one at a time
	if (deployedEnemies[numberOfEnemiesWhichFinishedMoving])
	{
		deployedEnemies[numberOfEnemiesWhichFinishedMoving]->ExecuteChosenAttack();
	}
	UE_LOG(LogTemp, Warning, TEXT("--------"));
}


float AAIManager::GetTotalStatFromDeployedEnemies(int statIndex_)
{
	float total = 0.0f;
	for (int i = 0; i < deployedEnemies.Num(); i++)
	{
		if(deployedEnemies[i])
			total += deployedEnemies[i]->GetStat(statIndex_);
	}

	return total;
}

AGridCharacter* AAIManager::GetEnemyWithHighestStat(int statIndex_)
{
	int max = 0;
	AGridCharacter* result = nullptr;
	for (int i = 0; i < deployedEnemies.Num(); i++)
	{
		if (deployedEnemies[i])
		{
			if (deployedEnemies[i]->GetStat(statIndex_) > max)
			{
				result = deployedEnemies[i];
				max = deployedEnemies[i]->GetStat(statIndex_);
			}
		}
	}

	return result;
}
AGridCharacter* AAIManager::GetEnemyWithLowestStat(int statIndex_)
{
	int min = 10000;
	AGridCharacter* result = nullptr;
	for (int i = 0; i < deployedEnemies.Num(); i++)
	{
		if (deployedEnemies[i])
		{
			if (deployedEnemies[i]->GetStat(statIndex_) < min)
			{
				result = deployedEnemies[i];
				min = deployedEnemies[i]->GetStat(statIndex_);
			}
		}
	}

	return result;
}
