// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"
#include "../BattleManager.h"
#include "../Grid/GridManager.h"
#include "../Grid/Tile.h"
#include "Engine/World.h"
#include "Intermediary/Intermediate.h"
#include "EnemyBaseGridCharacter.h"
#include "TimerManager.h"


// Sets default values
AAIManager::AAIManager()
{
	PrimaryActorTick.bCanEverTick = false;
	numberOfEnemiesWhichFinishedMoving = -1;
	numberOfEnemiesToldToMove = 0;
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
	numberOfEnemiesWhichFinishedMoving = -1; //Reset
	TellDeployedEnemiesToMove();
}

void AAIManager::TellDeployedEnemiesToMove()
{
	if (deployedEnemies.Num() > 0)
	{
		if (numberOfEnemiesToldToMove < deployedEnemies.Num())
		{
			deployedEnemies[numberOfEnemiesToldToMove]->StartEnemyTurn();
			numberOfEnemiesToldToMove++;
			FTimerHandle moveDelayHandle;
			GetWorld()->GetTimerManager().SetTimer(moveDelayHandle, this, &AAIManager::TellDeployedEnemiesToMove, 0.3f, false);
		}
		else
		{
			numberOfEnemiesToldToMove = 0;
		}
	}
}

void AAIManager::SetBattleAndGridManager(ABattleManager* btl_, AGridManager* grid_)
{
	//Called by the battle manager at begin play
	btlManager = btl_;
	gridManager = grid_;
	TArray<ATile*> enemyDeploymentTiles;

	//Get the next opponent
	nextOp = Intermediate::GetInstance()->GetNextOpponent();
	int nodeIndex = 0;

	if (gridManager)
	{
		gridManager->ClearHighlighted();

		if (depNodes.Num() > 0)
		{
			while (deployedEnemies.Num() < nextOp.numberOfTroops && nodeIndex < depNodes.Num()) //Potential issue: What if the total num of enemies exceeds the total num of enemies allowed on nodes?
			{
				//Highlight the tiles for the current node
				if (depNodes[nodeIndex].currentNumOfEnemiesForThisNode < depNodes[nodeIndex].maxNumOfEnemiesForThisNode)
				{
					gridManager->HighlightDeploymentTiles(depNodes[nodeIndex].enemyRowDeploymentIndex, depNodes[nodeIndex].enemyDeploymentOffset,
						depNodes[nodeIndex].enemyDeploymentRowSpeed, depNodes[nodeIndex].enemyDeploymentDepth);

					enemyDeploymentTiles = gridManager->GetHighlightedTiles();
				}				

				//Go through the tiles 
				for (;depNodes[nodeIndex].currentNumOfEnemiesForThisNode < depNodes[nodeIndex].maxNumOfEnemiesForThisNode && deployedEnemies.Num() < nextOp.numberOfTroops;)
				{
					int enemyIndex = FMath::RandRange(0, enemiesBPs.Num() - 1);
					int tileIndex = FMath::RandRange(0, enemyDeploymentTiles.Num() - 1);

					if (enemyDeploymentTiles[tileIndex]->GetOccupied()) //Make sure the tile was never selected before to avoid two enemies being spawned on the same tile
						continue;		// Potential infinite loop: If all the tiles are occupied, then infinite loop


					//Spawn the enemy and pass on the needed information
					FVector loc = enemyDeploymentTiles[tileIndex]->GetActorLocation();
					loc.Z += 50.0f;
					AEnemyBaseGridCharacter* enemy = GetWorld()->SpawnActor<AEnemyBaseGridCharacter>(enemiesBPs[enemyIndex], loc, FRotator(0.0f, 180.0f, 0.0f));

					if (enemy)
					{
						enemy->SetManagers(this, gridManager, btlManager);
						deployedEnemies.Push(enemy);
						enemyDeploymentTiles[tileIndex]->SetOccupied(true);
						depNodes[nodeIndex].currentNumOfEnemiesForThisNode++;
						enemy->bpID = enemyIndex;
					}
				}

				//Reset and get ready for the next node
				if (enemyDeploymentTiles.Num() > 0)
					enemyDeploymentTiles.Empty();

				gridManager->ClearHighlighted();
				nodeIndex++;
			}
		}
	}
}

void AAIManager::FinishedMoving()
{
	//Called by enemies when they finish moving. Used to know when to tell the enemies to attack
	if(numberOfEnemiesWhichFinishedMoving+1 <= deployedEnemies.Num() - 1) //Don't go above the total number of deployed enemies
		numberOfEnemiesWhichFinishedMoving++;

	//UE_LOG(LogTemp, Warning, TEXT("Called AI Manager finished moving %d , %d"), numberOfEnemiesWhichFinishedMoving, deployedEnemies.Num() - 1);
	if (numberOfEnemiesWhichFinishedMoving == (deployedEnemies.Num() - 1))
	{
		FTimerHandle attackDelayHandle;
		GetWorld()->GetTimerManager().SetTimer(attackDelayHandle, this, &AAIManager::OrderEnemiesToAttackPlayer, 0.7f, false);
	}
}
void AAIManager::FinishedAttacking()
{
	//When an enemy finishes attacking, this function is called so we can tell the next one to attack.
	numberOfEnemiesWhichFinishedMoving--;
	if (numberOfEnemiesWhichFinishedMoving >= 0 && numberOfEnemiesWhichFinishedMoving < deployedEnemies.Num())
	{
		OrderEnemiesToAttackPlayer();
	}
	else
	{ //All the enemies have finished their actions
		if (btlManager)
			btlManager->NextPhase();
	}
}

void AAIManager::OrderEnemiesToAttackPlayer()
{
	//TODO
	//Potential issue here when an enemy dies

	//Attacking happens one at a time
	if (numberOfEnemiesWhichFinishedMoving >= 0 && numberOfEnemiesWhichFinishedMoving < deployedEnemies.Num())
	{
		if (deployedEnemies[numberOfEnemiesWhichFinishedMoving])
		{
			deployedEnemies[numberOfEnemiesWhichFinishedMoving]->ExecuteChosenAction();
		}
	}

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

void AAIManager::HandleEnemyDeath(AEnemyBaseGridCharacter* enemy_)
{
	if (deployedEnemies.Contains(enemy_))
	{
		deployedEnemies.Remove(enemy_);
	}
	if (deployedEnemies.Num() == 0)
	{
		if (btlManager)
			btlManager->EndBattle(true); //Victory
	}
}

void AAIManager::TellEnemiesToCheckChangedStats()
{
	//Called by the battlemanager if the affectedParty in the intermeidate is set to 0 meaning enemies are getting a stat change at the beginning of the battle
	for (auto e : deployedEnemies)
	{
		e->CheckChangeStats();
	}
}
