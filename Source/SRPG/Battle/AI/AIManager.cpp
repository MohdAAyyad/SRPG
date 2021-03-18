// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"
#include "../BattleManager.h"
#include "../Grid/GridManager.h"
#include "../Grid/Tile.h"
#include "Engine/World.h"
#include "Intermediary/Intermediate.h"
#include "EnemyBaseGridCharacter.h"
#include "TimerManager.h"
#include "../Crowd/BattleCrowd.h"
#include "UnrealNetwork.h"


// Sets default values
AAIManager::AAIManager()
{
	PrimaryActorTick.bCanEverTick = false;
	numberOfEnemiesWhichFinishedMoving = -1;
	numberOfEnemiesToldToMove = 0;
	btlManager = nullptr;
	gridManager = nullptr;
	crdManager = nullptr;
	bReplicates = true;
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


void AAIManager::SetBattleGridCrdManagers(ABattleManager* btl_, AGridManager* grid_, ABattleCrowd* cref_)
{
	//Called by the battle manager at begin play
	btlManager = btl_;
	gridManager = grid_;
	crdManager = cref_;

	DeployEnemies();

}


void AAIManager::DeployEnemies()
{

	TArray<ATile*> enemyDeploymentTiles;
	//Get the next opponent
	nextOp = Intermediate::GetInstance()->GetNextOpponent();
	//UE_LOG(LogTemp, Warning, TEXT("Num of troops AI %d"), nextOp.numberOfTroops);

	int nodeIndex = 0;

	if (gridManager)
	{
		gridManager->ClearHighlighted();

		if (depNodes.Num() > 0)
		{
			//Loop through the available nodes to spawn the regular enemies first
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
				for (; deployedEnemies.Num() < nextOp.numberOfTroops && nodeIndex < depNodes.Num();)
				{
					int enemyIndex = FMath::RandRange(0, enemiesBPs.Num() - 1);
					int tileIndex = FMath::RandRange(0, enemyDeploymentTiles.Num() - 1);

					if (enemyDeploymentTiles[tileIndex]->GetOccupied() || !enemyDeploymentTiles[tileIndex]->GetTraversable()) //Make sure the tile was never selected before to avoid two enemies being spawned on the same tile
						continue;		// Potential infinite loop: If all the tiles are occupied, then infinite loop


					//Spawn the enemy and pass on the needed information
					FVector loc = enemyDeploymentTiles[tileIndex]->GetActorLocation();
					loc.Z += 50.0f;
					AEnemyBaseGridCharacter* enemy = GetWorld()->SpawnActor<AEnemyBaseGridCharacter>(enemiesBPs[enemyIndex], loc, FRotator(0.0f, 180.0f, 0.0f));

					if (enemy)
					{
						enemy->bpID = enemyIndex;
						enemy->SetManagers(this, gridManager, btlManager, crdManager);
						deployedEnemies.Push(enemy);
						enemyDeploymentTiles[tileIndex]->SetOccupied(true);						
						depNodes[nodeIndex].currentNumOfEnemiesForThisNode++;

						//If we have used up all the slots on this node, go to the next one
						if (depNodes[nodeIndex].currentNumOfEnemiesForThisNode >= depNodes[nodeIndex].maxNumOfEnemiesForThisNode)
						{
							//Reset and get ready for the next node
							if (enemyDeploymentTiles.Num() > 0)
								enemyDeploymentTiles.Empty();

							gridManager->ClearHighlighted();
							nodeIndex++;
							break;
						}
					}
				}


			}
		}
		gridManager->ClearHighlighted();
	}
}

void AAIManager::BeginEnemyTurn()
{
	numberOfEnemiesToldToMove = 0; //Reset
	numberOfEnemiesWhichFinishedMoving = -1; //Reset
	actionQueue.Empty();
	TellEnemiesToMove();
}

void AAIManager::TellEnemiesToMove()
{
	if (deployedEnemies.Num() > 0)
	{
		if (numberOfEnemiesToldToMove <= nextOp.numberOfTroops && numberOfEnemiesToldToMove < deployedEnemies.Num())
		{
			deployedEnemies[numberOfEnemiesToldToMove]->StartEnemyTurn();
			actionQueue.Enqueue(deployedEnemies[numberOfEnemiesToldToMove]); //Add the enemy to the queue
			numberOfEnemiesToldToMove++;
			FTimerHandle moveDelayHandle;
			GetWorld()->GetTimerManager().SetTimer(moveDelayHandle, this, &AAIManager::TellEnemiesToMove, 0.8f, false);
		}
	}
	//We do not return the numberOfEnemiesToldToMove variable to zero as we will need this value to be bigger than zero later on to tell support enemies to move
}

void AAIManager::FinishedMoving()
{
	//Called by enemies when they finish moving. Used to know when to tell the enemies to attack
	if(numberOfEnemiesWhichFinishedMoving+1 <= deployedEnemies.Num() - 1) //Don't go above the total number of deployed enemies
		numberOfEnemiesWhichFinishedMoving++;

	if (numberOfEnemiesWhichFinishedMoving == deployedEnemies.Num() - 1)
	{
		OrderEnemiesToExecuteAction();
	}
}
void AAIManager::FinishedAttacking()
{
	//When an enemy finishes attacking, this function is called so we can tell the next one to attack.
	numberOfEnemiesWhichFinishedMoving--;

	actionQueue.Pop();
	if (!actionQueue.IsEmpty()) //If we still have enemies in the queue, order them to attack
	{
		OrderEnemiesToExecuteAction();
	}
	else
	{
		if (btlManager)
			btlManager->NextPhase();
	}
}

void AAIManager::OrderEnemiesToExecuteAction()
{
	//Attacking happens one at a time
	AEnemyBaseGridCharacter* nextEnemyToAttack = nullptr;

	if (actionQueue.Peek(nextEnemyToAttack))
	{
		nextEnemyToAttack->ExecuteChosenAction();
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

AGridCharacter* AAIManager::GetEnemyWithHighestStat(int statIndex_, AGridCharacter* notThisCharacter_)
{
	int max = 0;
	AGridCharacter* result = nullptr;
	for (int i = 0; i < deployedEnemies.Num(); i++)
	{
		if (deployedEnemies[i])
		{
			if (deployedEnemies[i]->GetStat(statIndex_) > max && deployedEnemies[i] != notThisCharacter_)
			{
				result = deployedEnemies[i];
				max = deployedEnemies[i]->GetStat(statIndex_);
			}
		}
	}

	return result;
}
AGridCharacter* AAIManager::GetEnemyWithLowestStat(int statIndex_, AGridCharacter* notThisCharacter_)
{
	int min = 10000;
	AGridCharacter* result = nullptr;
	for (int i = 0; i < deployedEnemies.Num(); i++)
	{
		if (deployedEnemies[i])
		{
			if (deployedEnemies[i]->GetStat(statIndex_) < min && deployedEnemies[i] != notThisCharacter_)
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
		nextOp.numberOfTroops--;
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

TArray<AEnemyBaseGridCharacter*> AAIManager::GetDeployedEnemies()
{
	return deployedEnemies;
}


void AAIManager::ActivateOutlines(bool value_)
{
	for (int i = 0; i < deployedEnemies.Num(); i++)
	{
		if(deployedEnemies[i])
			deployedEnemies[i]->ActivateOutline(value_);
	}
}