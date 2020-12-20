// Fill out your copyright notice in the Description page of Project Settings.


#include "SupportDecisionComp.h"
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

USupportDecisionComp::USupportDecisionComp()
{
	bTargetIsHostile = false;
}
AGridCharacter* USupportDecisionComp::FindTheOptimalTargetCharacter()
{
	//UE_LOG(LogTemp, Warning, TEXT("Find Optimal Target has been called in support"));
	bWillUseSkill = false;
	skillType = -1;
	defenseSkillWithTheMaxRangeIndex = -1; //To make sure the support enemy picks the correct skill later on 
	return FindHealTarget(nullptr);
}
AGridCharacter* USupportDecisionComp::FindHealTarget(AEnemyBaseGridCharacter* ignoreThisTarget)
{
	//Supports look for fellow enemies to either heal or buff
	if (aiManager && ownerEnemy)
	{
		TArray<AEnemyBaseGridCharacter*> echars = aiManager->GetDeployedEnemies();
		FVector myLoc = ownerEnemy->GetActorLocation();
		AEnemyBaseGridCharacter* healTarget = ignoreThisTarget;
		if (echars.Num() > 0)
		{
			float hp_ = 0.0f;
			float maxHp_ = 0.0f;

			for (int i = 0; i < echars.Num(); i++)
			{
				if (echars[i])
				{
					if (echars[i] != healTarget)
					{
						float distance = (myLoc - echars[i]->GetActorLocation()).Size();
						if (distance <= RAD_SUPP) //If they're within our target radius, then check their HP
						{
							//UE_LOG(LogTemp, Warning, TEXT("Checking heal distance and it is lower"));
							hp_ = echars[i]->GetStat(STAT_HP);
							maxHp_ = echars[i]->GetStat(-1);
							if (hp_ <= (maxHp_ * HP_SUPP)) //If they're less than the support threshold, then that's our target
							{
								//UE_LOG(LogTemp, Warning, TEXT("Checking heal threshold and it is good"));
								healTarget = echars[i];
								break;
							}
						}
					}
				}
			}
		}
		if (healTarget)
		{ //Were we able to find a target within our radius and below the HP threshold?
			//UE_LOG(LogTemp, Warning, TEXT("Support: We have a heal target"));
			currentTarget = healTarget;
			currentTarget->YouAreTargetedByMeNow(ownerEnemy);
			return currentTarget;
		}
		else //Otherwise, find a character to buff
		{
			//UE_LOG(LogTemp, Warning, TEXT("Gonna try to find a support target"));
			return FindBuffTarget(echars, myLoc);
		}
	}
	return nullptr;
}
AGridCharacter* USupportDecisionComp::FindBuffTarget(TArray<AEnemyBaseGridCharacter*> echars, FVector myLoc)
{
	int targetStat = -1;
	UStatsComponent* statsComp = ownerEnemy->GetStatsComp();
	AEnemyBaseGridCharacter* supportTarget = nullptr;
	//Check if we have any skills that buff stats

	if (defenseSkills.Num() > 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Support: We have buff skills"));
	}
	for (int i = 0; i < defenseSkills.Num(); i++)
	{
		//Find the first usable buff stat
		if (defenseSkills[i].statIndex != STAT_HP && defenseSkills[i].pip <= statsComp->GetStatValue(STAT_PIP))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Found a usable support skill"));
			targetStat = defenseSkills[i].statIndex;
			defenseSkillWithTheMaxRangeIndex = i;
			break;
		}
	}

	//Look for an enemy within range and who has was not already buffed
	if (targetStat != -1) //Make sure we got a stat first
	{
		if (echars.Num() > 0)
		{
			for (int i = 0; i < echars.Num(); i++)
			{
				if (echars[i])
				{
					float distance = (myLoc - echars[i]->GetActorLocation()).Size();
					if (distance <= RAD_SUPP) //If they're within our target radius, then check their HP
					{
						if (currentTarget != echars[i] && !echars[i]->GetStatsComp()->HasThisStatBeenBuffed(targetStat)) //Make sure this character did not have this particular stat buffed before
						{
							//UE_LOG(LogTemp, Warning, TEXT("Found a support target"));
							supportTarget = echars[i];
							break;
						}
					}
				}
			}
		}
		if (supportTarget)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Returning support target"));
			currentTarget = supportTarget;
			currentTarget->YouAreTargetedByMeNow(ownerEnemy);
			return currentTarget;
		}
	}
	else //We could not find any usable buffs skills and since we're here, couldn't find  anybody to heal either so
	{
		//UE_LOG(LogTemp, Warning, TEXT("Gonna go for the closest least health enemy"));
		//Move closer to the enemy with the least health 
		FindClosestAllyWithTheLeastHealth(echars, myLoc);
	}

	return nullptr;
}

AGridCharacter* USupportDecisionComp::FindClosestAllyWithTheLeastHealth(TArray<AEnemyBaseGridCharacter*> echars, FVector myLoc)
{
	//UE_LOG(LogTemp, Warning, TEXT("Support: Find closest ally with least health"));
	//Move closer to the enemy with the least health 
	defenseSkillWithTheMaxRangeIndex = -2;
	float minHealth = FLT_MAX;
	int targetIndex = -1;
	if (echars.Num() > 0)
	{
		for (int i = 0; i < echars.Num(); i++)
		{
			if (echars[i])
			{
				if (echars[i]->GetStat(STAT_HP) < minHealth && echars[i] != ownerEnemy)
				{
					minHealth = echars[i]->GetStat(STAT_HP);
					targetIndex = i;
				}
			}
		}
	}
	currentTarget = echars[targetIndex];
	currentTarget->YouAreTargetedByMeNow(ownerEnemy);
	return currentTarget;
}


ATile* USupportDecisionComp::FindOptimalTargetTile(ATile* myTile_)
{
	if (myTile_)
	{
		if (defenseSkillWithTheMaxRangeIndex == -1) //We have not decided on a buff skill yet i.e. we decided to heal
		{
			//	UE_LOG(LogTemp, Warning, TEXT("Gonna look for heal tile"));
			if (currentTarget) 	//Checks if we have a target reference first, otherwise, move to target tile
			{
				UStatsComponent* statsComp = ownerEnemy->GetStatsComp();
				FindDefensiveSkillThatUpdatesThisStatAndHasTheHighestRange(STAT_HP, statsComp->GetStatValue(STAT_PIP));
				ATile* targetTile = nullptr;
				if (defenseSkillWithTheMaxRangeIndex != -1)
					targetTile = ChooseTileBasedOnPossibleSupportActions(myTile_); //If we got a heal skill, then find a possible tile

				//If we could not find a feasable tile, then find a different target
				if (!targetTile)
				{
					//	UE_LOG(LogTemp, Warning, TEXT("Tried to heal but couldn't get a tile. Gonna attempt to find another target to heal"));
					currentTarget->YouAreNoLongerTargetedByMe(ownerEnemy);
				ownerEnemy->UpdateTargetCharacter(FindHealTarget(Cast<AEnemyBaseGridCharacter> (currentTarget)));
					return FindOptimalTargetTile(myTile_); //We've switched to a different target so run the function again and find a tile
				}
				else if (targetTile == myTile_) //We're not gonna be moving
				{
					return nullptr; //nullptr is interpreted in the ownerEnemy as finished moving
				}
				else
				{
					//	UE_LOG(LogTemp, Warning, TEXT("Found a good tile to support from"));
					return targetTile;
				}
			}
			else//We don't have a player target, find the optimal one per the enum rules.
			{
				//	UE_LOG(LogTemp, Warning, TEXT("We don't really have a support target"));					
				ownerEnemy->UpdateTargetCharacter(FindTheOptimalTargetCharacter());
				return FindOptimalTargetTile(myTile_); //We've got a target now so run the function again
			}

		}
		else if (defenseSkillWithTheMaxRangeIndex == -2) //We decided to go to the enemy with the least health
		{
			//	UE_LOG(LogTemp, Warning, TEXT("Gonna get closer tile"));
			if (currentTarget)
			{
				bWillUseSkill = false;
				return currentTarget->GetMyTile();
			}
		}
		else //Buff skill
		{
			if (currentTarget)
			{
				//	UE_LOG(LogTemp, Warning, TEXT("Trying to find tile for support skill"));
				ATile* targetTile = ChooseTileBasedOnPossibleSupportActions(myTile_);

				//If we could not find a feasable tile, then find the tile closes to the enemy with the least heal
				if (!targetTile)
				{
					//	UE_LOG(LogTemp, Warning, TEXT("Tried to support but not tile. Let's look for a different target :("));
					//currentTarget->YouAreNoLongerTargetedByMe(ownerEnemy);
					//ownerEnemy->UpdateTargetCharacter(FindHealTarget(Cast<AEnemyBaseGridCharacter> (currentTarget)));
					//HEEEERRREEEE FIIXXXX HEEERRREEEE
					//return FindOptimalTargetTile(myTile_); //We've switched to a different target so run the function again and find a tile
					return nullptr;
				}
				else if (targetTile == myTile_) //We're not gonna be moving
				{
					//	UE_LOG(LogTemp, Warning, TEXT("It's my tile"));
					return nullptr; //nullptr is interpreted in the ownerEnemy as finished moving
				}
				else
				{
					//	UE_LOG(LogTemp, Warning, TEXT("Found a good tile"));
					return targetTile;
				}
			}
			else
			{
				//	UE_LOG(LogTemp, Warning, TEXT("Gonna find another character"));
					//We don't have a player target, find the optimal one per the enum rules.
				ownerEnemy->UpdateTargetCharacter(FindTheOptimalTargetCharacter());
				return FindOptimalTargetTile(myTile_); //We've got a target now so run the function again
			}

		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Tried to find support tile but I don't really have a tile myself"));
	return nullptr;
}