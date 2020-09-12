// Fill out your copyright notice in the Description page of Project Settings.


#include "DecisionComp.h"
#include "EnemyBaseGridCharacter.h"
#include "AIManager.h"
#include "Battle/BattleManager.h"
#include "Definitions.h"
#include "Battle/Player/PlayerGridCharacter.h"

// Sets default values for this component's properties
UDecisionComp::UDecisionComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bTargetIsHostile = true;
	currentPattern = EPatterns::DEFAULT;
	aiManager = nullptr;
	btlManager = nullptr;
	ownerEnemy = nullptr;
	targetRadius = 0; //Updated based on the pattern
	// ...
}

// Called when the game starts
void UDecisionComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UDecisionComp::SetRefs(AAIManager* ai_, ABattleManager* btl_, AEnemyBaseGridCharacter* enemy_)
{
	aiManager = ai_;
	btlManager = btl_;
	ownerEnemy = enemy_;
}
void UDecisionComp::UpdatePattrn(int level_, bool healer_)
{
	if (healer_)
	{
		currentPattern = EPatterns::HEALER;
	}
	else
	{
		int maxPotentialPattern = 0;
		if (level_ >= 1 && level_ < PAT_ONE)
			maxPotentialPattern = 0;
		else if (level_ >= PAT_ONE && level_ < PAT_TWO)
			maxPotentialPattern = 1;
		else if (level_ >= PAT_TWO && level_ < PAT_THR)
			maxPotentialPattern = 2;
		else if (level_ >= PAT_THR)
			maxPotentialPattern = 3;

		currentPattern = static_cast<EPatterns>(FMath::RandRange(0, maxPotentialPattern));
	}
}
AGridCharacter* UDecisionComp::FindTheOptimalTarget()
{
	switch (currentPattern)
	{
	case EPatterns::DEFAULT:
		return FindDefaultTarget();
	case EPatterns::ASSASSIN:
		targetRadius = RAD_ASS;
		return FindAssassinTarget();
	case EPatterns::FOLLOWER:
		targetRadius = RAD_FOL;
		return FindFollowerTarget();
	case EPatterns::PEOPLEPERSON:
		targetRadius = RAD_PP;
		return PeoplePersonEffect();
	case EPatterns::HEALER:
		targetRadius = RAD_HEAL;
		return FindHealerTarget();
	}
	return nullptr;
}
bool UDecisionComp::IsMyTargetHostile()
{
	//Depends on the pattern. Used by enemies to know if they should use offensive or support skills

	return bTargetIsHostile;
}

AGridCharacter* UDecisionComp::FindAnotherTarget(AGridCharacter* target_)
{
	currentTarget = target_;
	return FindTheOptimalTarget();
}
AGridCharacter* UDecisionComp::FindDefaultTarget()
{
	//Not using dijkstra as precision is not important. We're assuming that the displacement is the shortest distance without checking the actual path through the tiles
	if (btlManager && ownerEnemy)
	{
		TArray<APlayerGridCharacter*> pchars = btlManager->GetDeployedPlayers();
		FVector myLoc = ownerEnemy->GetActorLocation();
		APlayerGridCharacter* target = Cast<APlayerGridCharacter>(currentTarget);
		if (pchars.Num() > 0)
		{
			float min = 10000.0f;
			for (int i = 0; i < pchars.Num(); i++)
			{
				//Get the next closest target. Mainly used when all the range tiles are occupied and the enemy needs a new target
				float distance = (myLoc - pchars[i]->GetActorLocation()).Size();
				if (distance < min && target != pchars[i]) //Making sure the target and the new target are not the same thing. Target is eithe nullptr or not in which case, findanothertarget was called
				{
					target = pchars[i];
					min = distance;
				}
			}
		}
		currentTarget = nullptr; // Reset
		return Cast<AGridCharacter>(target);
	}
	return nullptr;
}

AGridCharacter* UDecisionComp::FindDefaultTarget(TArray<APlayerGridCharacter*> pchars, FVector myLoc)
{
	//Not using dijkstra as precision is not important. We're assuming that the displacement is the shortest distance without checking the actual path through the tiles
	if (btlManager && ownerEnemy)
	{
		APlayerGridCharacter* target = Cast<APlayerGridCharacter>(currentTarget);
		if (pchars.Num() > 0)
		{
			float min = 10000.0f;
			for (int i = 0; i < pchars.Num(); i++)
			{
				//Get the next closest target. Mainly used when all the range tiles are occupied and the enemy needs a new target
				float distance = (myLoc - pchars[i]->GetActorLocation()).Size();
				if (distance < min && target != pchars[i])
				{
					target = pchars[i];
					min = distance;
				}
			}
		}
		currentTarget = nullptr;
		return Cast<AGridCharacter>(target);
	}
	return nullptr;

}
AGridCharacter* UDecisionComp::FindAssassinTarget()
{
	//Assassins look for the player characters with the least HP within their radius
	if (btlManager && ownerEnemy)
	{
		TArray<APlayerGridCharacter*> pchars = btlManager->GetDeployedPlayers();
		FVector myLoc = ownerEnemy->GetActorLocation();
		APlayerGridCharacter* target = Cast<APlayerGridCharacter>(currentTarget);
		if (pchars.Num() > 0)
		{
			float threshold = HP_ASS;
			float hp_ = 0.0f;
			float maxHp_ = 0.0f;

			for (int i = 0; i < pchars.Num(); i++)
			{
				float distance = (myLoc - pchars[i]->GetActorLocation()).Size();
				if (distance <= targetRadius) //If they're within our target radius, then check their HP
				{
					hp_ = pchars[i]->GetStat(STAT_HP);
					maxHp_ = pchars[i]->GetStat(-1);
					if (hp_ <= (maxHp_ * threshold) && target != pchars[i]) //If they're less than the threshold, then that's our target
					{
						target = pchars[i];
						break;
					}
				}
			}
		}
		if (target)
		{ //Were we able to find a target within our radius and below the HP threshold?
			currentTarget = nullptr;
			return Cast<AGridCharacter>(target);
		}
		else //Otherwise, just go default
		{
			FindDefaultTarget(pchars, myLoc);
		}
	}
	return nullptr;
}
AGridCharacter* UDecisionComp::FindFollowerTarget()
{
	return nullptr;
}
AGridCharacter* UDecisionComp::PeoplePersonEffect()
{
	return nullptr;
}
AGridCharacter* UDecisionComp::FindHealerTarget()
{
	return nullptr;
}
