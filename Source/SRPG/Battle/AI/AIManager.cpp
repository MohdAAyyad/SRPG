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

}

// Called when the game starts or when spawned
void AAIManager::BeginPlay()
{
	Super::BeginPlay();
	if (gridManager)
	{
		gridManager->HighlightDeploymentTiles(enemyRowDeploymentIndex, enemyDeploymentOffset,
											  enemyDeploymentRowSpeed, enemyDeploymentDepth);

		enemyDeploymentTiles = gridManager->GetHighlightedTiles();
		gridManager->ClearHighlighted();

		for (int i = 0; i < thisIntShouldBeReplacedWithTheSOpponentNumberOfTroopsVariable; i++)
		{
			int enemyIndex  = FMath::RandRange(0, enemiesBPs.Num() - 1);
			int tileIndex =   FMath::RandRange(0, enemyDeploymentTiles.Num() - 1);
			AEnemyBaseGridCharacter* enemy = GetWorld()->SpawnActor<AEnemyBaseGridCharacter>(enemiesBPs[enemyIndex], enemyDeploymentTiles[tileIndex]->GetActorLocation(), FRotator(0.0f,180.0f,0.0f));

			if (enemy)
			{
				enemy->SetManagers(this,gridManager);
				enemy->PickATargetTile();
			}
		}
	}
	
}

// Called every frame
void AAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIManager::BeginEnemyTurn()
{

}

