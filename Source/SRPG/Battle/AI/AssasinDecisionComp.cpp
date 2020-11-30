// Fill out your copyright notice in the Description page of Project Settings.


#include "AssasinDecisionComp.h"
#include "EnemyBaseGridCharacter.h"
#include "AIManager.h"
#include "Battle/BattleManager.h"
#include "Definitions.h"
#include "Battle/Player/PlayerGridCharacter.h"
#include "../Grid/GridManager.h"
#include "../Grid/Tile.h"
#include "../../ExternalFileReader/ExternalFileReader.h"
#include "../StatsComponent.h"
#include "../PathComponent.h"
#include "Definitions.h"

UAssasinDecisionComp::UAssasinDecisionComp():UDecisionComp()
{
	bTargetIsHostile = true;
}
AGridCharacter* UAssasinDecisionComp::FindTheOptimalTargetCharacter()
{
	bWillUseSkill = false;
	skillType = -1;
	return FindTargetWithLeastHealth(nullptr);
}
AGridCharacter* UAssasinDecisionComp::FindTargetWithLeastHealth(APlayerGridCharacter* ignoreThis)
{
	if (btlManager && ownerEnemy)
	{
		TArray<APlayerGridCharacter*> pchars = btlManager->GetDeployedPlayers();
		FVector myLoc = ownerEnemy->GetActorLocation();
		APlayerGridCharacter* target = ignoreThis;
		if (pchars.Num() > 0)
		{
			float hp_ = 0.0f;
			float maxHp_ = 0.0f;

			if (!bPersistant || !currentTarget) //Only look for a new target if you're not persistent or if you don't have a target
			{
				for (int i = 0; i < pchars.Num(); i++)
				{
					if (pchars[i])
					{
						float distance = (myLoc - pchars[i]->GetActorLocation()).Size();
						if (distance <= RAD_ASSA) //If they're within our target radius, then check their HP
						{
							hp_ = pchars[i]->GetStat(STAT_HP);
							maxHp_ = pchars[i]->GetStat(-1);
							if (hp_ <= (maxHp_ * HP_ASSA) && target != pchars[i]) //If they're less than the threshold, then that's our target
							{
								target = pchars[i];
								break;
							}
						}
					}
				}

				if (target)
				{ //Were we able to find a target within our radius and below the HP threshold?
					currentTarget = target;
					currentTarget->YouAreTargetedByMeNow(ownerEnemy);
					return currentTarget;
				}
			}
		}

		//We won't reach this line unless you don't have a target yet
		if(!currentTarget) //Otherwise, find the closest target
		{
			return FindClosestTarget(pchars, myLoc);
		}
	}
	return nullptr;
}
AGridCharacter* UAssasinDecisionComp::FindClosestTarget(TArray<APlayerGridCharacter*> pchars, FVector myLoc)
{
	if (btlManager && ownerEnemy)
	{
		APlayerGridCharacter* target = Cast<APlayerGridCharacter>(currentTarget);

		if (pchars.Num() > 0)
		{
			float min = FLT_MAX;
			for (int i = 0; i < pchars.Num(); i++)
			{
				float distance = (myLoc - pchars[i]->GetActorLocation()).Size();
				if (distance < min && target != pchars[i])
				{
					target = pchars[i];
					min = distance;
				}
			}
		}
		currentTarget = target;
		currentTarget->YouAreTargetedByMeNow(ownerEnemy);
		return currentTarget;
	}
	return nullptr;
}
ATile* UAssasinDecisionComp::FindOptimalTargetTile(ATile* myTile_)
{
	if (currentTarget) 	//Checks if we have a player reference first, otherwise, move to target tile
	{
		ATile* targetTile = ChooseTileBasedOnPossibleOffenseActions(myTile_); //Attacking is the default pattern

		//If all range tiles are occupied, Find another target
		if (!targetTile)
		{
			AGridCharacter* ignoreThisTarget = currentTarget;
			if (bPersistant) //If this enemy is persistent, we need to tell it to find another target and to do that, the current target needs to be made null but we still need to pass it in to avoid it
			{
				currentTarget = nullptr;
			}
			FindTargetWithLeastHealth(Cast<APlayerGridCharacter>(ignoreThisTarget));
			return FindOptimalTargetTile(myTile_); //We've switched to a different target so run the function again and find a tile
		}
		else if (targetTile == myTile_) //We're not gonna be moving
		{
			return nullptr;
		}
		else
		{
			return targetTile;
		}
	}
	else
	{
		//We don't have a player target, find the optimal one per the enum rules.
		FindTheOptimalTargetCharacter();
		return FindOptimalTargetTile(myTile_); //We've got a target now so run the function again
	}

	return nullptr;
}
