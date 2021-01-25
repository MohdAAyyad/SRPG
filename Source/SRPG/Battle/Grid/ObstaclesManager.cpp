// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaclesManager.h"
#include "Obstacle.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../BattleManager.h"

// Sets default values
AObstaclesManager::AObstaclesManager()
{
	PrimaryActorTick.bCanEverTick = false;
	obstacleExistanceChance = 50.0f;
}

void AObstaclesManager::BeginPlay()
{
	Super::BeginPlay();
	
}


void AObstaclesManager::AddObstacle(AObstacle* obstacle_, bool persistanceObstacle_)
{
	if (!persistanceObstacle_) //If it can be removed then see if it stays or otherwise
	{
		float chance = FMath::RandRange(0.0f, 100.0f);

		if (chance <= obstacleExistanceChance) //Obstacle stays
		{
			if (!obstacles.Contains(obstacle_))
			{
				obstacles.Push(obstacle_);
			}
		}
		else //Otherwise, destroy the obstacle
		{
			if (obstacle_)
				obstacle_->DelayedDestroy();
		}

	}
	else //This obstacle cannot be removed
	{
		if (!obstacles.Contains(obstacle_))
		{
			obstacles.Push(obstacle_);
		}
	}
}

UParticleSystemComponent* AObstaclesManager::SpawnElemntalEmitterAtLocation(FVector loc_, CurrentElemntalStat elementStat_)
{
	//Called by elementals when they change state
	int index_ = GetEmitterIndex(elementStat_);

	if (index_ >= 0 && index_ < elementalEmitters.Num())
	{
		return UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), elementalEmitters[index_], loc_, FRotator::ZeroRotator);
	}
	return nullptr;
}

int AObstaclesManager::GetEmitterIndex(CurrentElemntalStat elementStat_)
{
	switch (elementStat_)
	{
	case CurrentElemntalStat::FIRE:
		return 0;
	case CurrentElemntalStat::FROZEN:
		return 1;
	case CurrentElemntalStat::PARALYSIS:
		return 2;
	case CurrentElemntalStat::EXPLOSION:
		return 3;
	case CurrentElemntalStat::STEAM:
		return 4;
	}
	return -1;
}

int AObstaclesManager::GetBattlePhase()
{
	if (btlManager)
		return btlManager->GetPhase();

	return -1;
}

void AObstaclesManager::TellObstaclesAPhaseHasPassed(int phase_)
{
	for (int i = 0; i < obstacles.Num(); i++)
	{
		if(obstacles[i])
			obstacles[i]->ATurnHasPassed(phase_);
	}

}

void AObstaclesManager::RemoveObstacle(AObstacle* obstacle_)
{
	for (int i = 0; i < obstacles.Num(); i++)
	{
		if (obstacles[i] == obstacle_)
		{
			obstacles.RemoveAt(i);
			obstacle_->Destroy();
			obstacle_ = nullptr;
		}
	}

	if(obstacle_) //Check for the obstacle again. This is needed when the obstacle is removed due to removal chance
		obstacle_->Destroy();
}

AObstacle* AObstaclesManager::GetAnObstacleAtRandom()
{
	if (obstacles.Num() > 0)
	{
		int index = FMath::RandRange(0, obstacles.Num() - 1);

		return obstacles[index];
	}
	return nullptr;
}

void AObstaclesManager::ActivateOutlines(bool value_)
{
	for (int i = 0; i < obstacles.Num(); i++)
	{
		if (obstacles[i])
			obstacles[i]->ActivateOutline(value_);
	}
}

