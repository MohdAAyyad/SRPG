// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleDecisionComp.h"
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

USimpleDecisionComp::USimpleDecisionComp():UDecisionComp()
{
	bTargetIsHostile = true;
}

AGridCharacter* USimpleDecisionComp::FindTheOptimalTargetCharacter()
{
	bWillUseSkill = false;
	skillType = -1;
	return FindClosestPlayerTarget(nullptr);

}

AGridCharacter* USimpleDecisionComp::FindClosestPlayerTarget(APlayerGridCharacter* ignoreTihs)
{
	//Not using dijkstra as precision is not important. We're assuming that the displacement is the shortest distance without checking the actual path through the tiles
	if (btlManager && ownerEnemy)
	{
		TArray<APlayerGridCharacter*> pchars = btlManager->GetDeployedPlayers();
		FVector myLoc = ownerEnemy->GetActorLocation();
		APlayerGridCharacter* target = ignoreTihs;

		if (currentTarget && !bPersistant)
			currentTarget->YouAreNoLongerTargetedByMe(ownerEnemy);

		if (!bPersistant || !currentTarget) //Only look for a new target if you're not persistent or if you don't have a target
		{
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
		}

		if (currentTarget)
			currentTarget->YouAreTargetedByMeNow(ownerEnemy);
		return currentTarget;
	}
	return nullptr;
}

ATile* USimpleDecisionComp::FindOptimalTargetTile(ATile* myTile_)
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
			FindClosestPlayerTarget(Cast<APlayerGridCharacter>(ignoreThisTarget));
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