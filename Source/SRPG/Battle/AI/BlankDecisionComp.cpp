// Fill out your copyright notice in the Description page of Project Settings.


#include "BlankDecisionComp.h"
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


UBlankDecisionComp::UBlankDecisionComp():UDecisionComp()
{
	offenseTargetRadiusInPx = 0.0f;
	supportTargetRadiusInPx = 0.0f;
	offenseChance = 100.0f;
	offenseTargetRadiusInTiles = 0;
	supportTargetRadiusInTiles = 0;
	bInclinedToAttack = false;
	bInclinedToAttack = false;
	findOffenseTarget = nullptr;
	findOffenseTile = nullptr;
	findSupportTarget = nullptr;
	findSupportTile = nullptr;
	bChosenToAttack = true;
}

void UBlankDecisionComp::BeginPlay()
{
	Super::BeginPlay();
	UpdateInclination();
	UpdateFunctionPointers();

	offenseTargetRadiusInPx = static_cast<float>(offenseTargetRadiusInTiles) * TILE_SIZE;
	supportTargetRadiusInPx = static_cast<float>(supportTargetRadiusInTiles) * TILE_SIZE;
}

void UBlankDecisionComp::UpdateInclination()
{
	//Make sure the inclinations logic makes sense
	if ((bInclinedToAttack == true && offenseTargetRadiusInTiles <= 0) || (bInclinedToAttack == false && offenseTargetRadiusInTiles > 0))
	{
		bInclinedToAttack = false;
		offenseTargetRadiusInTiles = 0;
	}

	if ((bInclinedToSupport == true && supportTargetRadiusInTiles <= 0) || (bInclinedToSupport == false && supportTargetRadiusInTiles > 0))
	{
		bInclinedToSupport = false;
		supportTargetRadiusInTiles = 0;
		if (!bInclinedToAttack)
		{
			//if both are false at this point that means I have no target radius for either and the logic is not sound. Die.
			ownerEnemy->GridCharTakeDamage(FLT_MAX, ownerEnemy, false);
		}
	}

	//Both are true and their target raidus is higher than zero. Default to inclined to attack
	if ((bInclinedToAttack && bInclinedToSupport))
	{
		bInclinedToAttack = true;
		bInclinedToSupport = false;
	}

	if (bInclinedToAttack && supportTargetRadiusInTiles > 0) //Inclined to attack but can also support
	{
		offenseChance = 70.0f;
	}
	else if (bInclinedToSupport && offenseTargetRadiusInTiles > 0) //Inclined to support but can also attack
	{
		offenseChance = 30.0f;
	}
	else if (bInclinedToSupport && offenseTargetRadiusInTiles <= 0) //Will only support
	{
		offenseChance = 0.0f;
	}
}

void UBlankDecisionComp::UpdateFunctionPointers()
{
	if (offenseTargetRadiusInTiles > 0)
	{
		switch (offenseCondition)
		{
		case ETargetConditions::SMALLEST_DISTANCE:
			findOffenseTarget = &UBlankDecisionComp::FindOffenseTarget_SmallestDistance;
			break;
		case ETargetConditions::LONGEST_DISTANCE:
			findOffenseTarget = &UBlankDecisionComp::FindOffenseTarget_LongestDistance;
			break;
		case ETargetConditions::LOWEST_CONDITIONED_STAT:
			findOffenseTarget = &UBlankDecisionComp::FindOffenseTarget_LowestStat;
			break;
		case ETargetConditions::HIGHEST_CONDITIONED_STAT:
			findOffenseTarget = &UBlankDecisionComp::FindOffenseTarget_HighestStat;
			break;
		default:
			break;
		}

		//TODO
		//Add more offenseTileFunctions maybe
		findOffenseTile = &UBlankDecisionComp::FindOptimalOffenseTile;
	}

	if (supportTargetRadiusInTiles > 0)
	{
		switch (supportCondition)
		{
		case ETargetConditions::SMALLEST_DISTANCE:
			findSupportTarget = &UBlankDecisionComp::FindSupportTarget_SmallestDistance;
			break;
		case ETargetConditions::LONGEST_DISTANCE:
			findSupportTarget = &UBlankDecisionComp::FindSupportTarget_LongestDistance;
			break;
		case ETargetConditions::LOWEST_CONDITIONED_STAT:
			findSupportTarget = &UBlankDecisionComp::FindSupportTarget_LowestStat;
			break;
		case ETargetConditions::HIGHEST_CONDITIONED_STAT:
			findSupportTarget = &UBlankDecisionComp::FindSupportTarget_HighestStat;
			break;
		default:
			break;
		}

		//TODO
		//Add more supportTileFunctions maybe
		findSupportTile = &UBlankDecisionComp::FindOptimalSupportTile;
	}
}

AGridCharacter* UBlankDecisionComp::FindTheOptimalTargetCharacter()
{
	bWillUseSkill = false;
	skillType = -1;

	if (offenseChance < 100.0f  && offenseChance > 0.0f) //Only if we have a mix do we need to calculate the offense chance
	{
		float chance = FMath::RandRange(0.0f, 100.0f);
		if (chance <= offenseChance)
		{
			bChosenToAttack = true;
			return (this->*(findOffenseTarget))(nullptr);
		}
		else
		{
			bChosenToAttack = false;
			return (this->*(findSupportTarget))(nullptr);
		}
	}
	else if (offenseChance >= 100.0f) //Will only attack
	{
		bChosenToAttack = true;
		return (this->*(findOffenseTarget))(nullptr);
	}
	else if (offenseChance <= 0.0f) //Will only support
	{
		bChosenToAttack = false;
		defenseSkillWithTheMaxRangeIndex = -1;
		return (this->*(findSupportTarget))(nullptr);
	}
	return nullptr;
}
ATile* UBlankDecisionComp::FindOptimalTargetTile(ATile* myTile_)
{

	if (bChosenToAttack)
	{
		return (this->*(findOffenseTile))(myTile_);
	}
	else
	{
		return (this->*(findSupportTile))(myTile_);
	}
	return nullptr;
}

#pragma region FindTargetFunctions

#pragma region OffenseTarget


AGridCharacter* UBlankDecisionComp::FindOffenseTarget_SmallestDistance(AGridCharacter* ignoreThis_)
{
	if (btlManager && ownerEnemy)
	{
		TArray<APlayerGridCharacter*> pchars = btlManager->GetDeployedPlayers();
		FVector myLoc = ownerEnemy->GetActorLocation();
		APlayerGridCharacter* target = Cast<APlayerGridCharacter>(ignoreThis_);

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
					if (distance < offenseTargetRadiusInPx)
					{
						UE_LOG(LogTemp, Warning, TEXT("Blank: Found potential target"));
						if (distance < min && target != pchars[i])
						{
							target = pchars[i];
							min = distance;
						}
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
AGridCharacter* UBlankDecisionComp::FindOffenseTarget_LongestDistance(AGridCharacter* ignoreThis_)
{
	if (btlManager && ownerEnemy)
	{
		TArray<APlayerGridCharacter*> pchars = btlManager->GetDeployedPlayers();
		FVector myLoc = ownerEnemy->GetActorLocation();
		APlayerGridCharacter* target = Cast<APlayerGridCharacter>(ignoreThis_);

		if (currentTarget && !bPersistant)
			currentTarget->YouAreNoLongerTargetedByMe(ownerEnemy);

		if (!bPersistant || !currentTarget) //Only look for a new target if you're not persistent or if you don't have a target
		{
			if (pchars.Num() > 0)
			{
				float max = FLT_MIN;
				for (int i = 0; i < pchars.Num(); i++)
				{
					float distance = (myLoc - pchars[i]->GetActorLocation()).Size();
					if (distance < offenseTargetRadiusInPx)
					{
						if (distance > max && target != pchars[i])
						{
							target = pchars[i];
							max = distance;
						}
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
AGridCharacter* UBlankDecisionComp::FindOffenseTarget_LowestStat(AGridCharacter* ignoreThis_)
{
	if (btlManager && ownerEnemy)
	{
		TArray<APlayerGridCharacter*> pchars = btlManager->GetDeployedPlayers();
		FVector myLoc = ownerEnemy->GetActorLocation();
		APlayerGridCharacter* target = Cast<APlayerGridCharacter>(ignoreThis_);
		if (pchars.Num() > 0)
		{
			float stat_ = 0.0f;
			float min = FLT_MAX;
			if (!bPersistant || !currentTarget) //Only look for a new target if you're not persistent or if you don't have a target
			{
				for (int i = 0; i < pchars.Num(); i++)
				{
					if (pchars[i])
					{
						float distance = (myLoc - pchars[i]->GetActorLocation()).Size();
						if (distance <= offenseTargetRadiusInPx) //If they're within our target radius, then check their stat
						{
							stat_ = pchars[i]->GetStat(offenseConditionStatIndex);
							if (stat_ < min && target != pchars[i]) //If they're less than the threshold, then that's our target
							{
								target = pchars[i];
								min = stat_;
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
		if (!currentTarget) //Otherwise, find the closest target
		{
			return FindOffenseTarget_SmallestDistance(ignoreThis_);
		}
	}
	return nullptr;
}
AGridCharacter* UBlankDecisionComp::FindOffenseTarget_HighestStat(AGridCharacter* ignoreThis_)
{
	if (btlManager && ownerEnemy)
	{
		TArray<APlayerGridCharacter*> pchars = btlManager->GetDeployedPlayers();
		FVector myLoc = ownerEnemy->GetActorLocation();
		APlayerGridCharacter* target = Cast<APlayerGridCharacter>(ignoreThis_);
		if (pchars.Num() > 0)
		{
			float stat_ = 0.0f;
			float max = FLT_MIN;
			if (!bPersistant || !currentTarget) //Only look for a new target if you're not persistent or if you don't have a target
			{
				for (int i = 0; i < pchars.Num(); i++)
				{
					if (pchars[i])
					{
						float distance = (myLoc - pchars[i]->GetActorLocation()).Size();
						if (distance <= offenseTargetRadiusInPx) //If they're within our target radius, then check their stat
						{
							stat_ = pchars[i]->GetStat(offenseConditionStatIndex);
							if (stat_ > max && target != pchars[i]) //If they're less than the threshold, then that's our target
							{
								target = pchars[i];
								max = stat_;
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
		if (!currentTarget) //Otherwise, find the closest target
		{
			return FindOffenseTarget_SmallestDistance(ignoreThis_);
		}
	}
	return nullptr;
}

#pragma endregion

#pragma region SupportTarget


AGridCharacter* UBlankDecisionComp::FindSupportTarget_SmallestDistance(AGridCharacter* ignoreThis_)
{
	if (aiManager && ownerEnemy)
	{
		TArray<AEnemyBaseGridCharacter*> echars = aiManager->GetDeployedEnemies();
		FVector myLoc = ownerEnemy->GetActorLocation();
		AEnemyBaseGridCharacter* target = Cast<AEnemyBaseGridCharacter>(ignoreThis_);

		if (currentTarget && !bPersistant)
			currentTarget->YouAreNoLongerTargetedByMe(ownerEnemy);

		if (!bPersistant || !currentTarget) //Only look for a new target if you're not persistent or if you don't have a target
		{
			if (echars.Num() > 0)
			{
				float min = FLT_MAX;
				for (int i = 0; i < echars.Num(); i++)
				{
					float distance = (myLoc - echars[i]->GetActorLocation()).Size();
					if (distance < supportTargetRadiusInPx)
					{
						if (distance < min && target != echars[i])
						{
							target = echars[i];
							min = distance;
						}
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
AGridCharacter* UBlankDecisionComp::FindSupportTarget_LongestDistance(AGridCharacter* ignoreThis_)
{
	if (aiManager && ownerEnemy)
	{
		TArray<AEnemyBaseGridCharacter*> echars = aiManager->GetDeployedEnemies();
		FVector myLoc = ownerEnemy->GetActorLocation();
		AEnemyBaseGridCharacter* target = Cast<AEnemyBaseGridCharacter>(ignoreThis_);

		if (currentTarget && !bPersistant)
			currentTarget->YouAreNoLongerTargetedByMe(ownerEnemy);

		if (!bPersistant || !currentTarget) //Only look for a new target if you're not persistent or if you don't have a target
		{
			if (echars.Num() > 0)
			{
				float max = FLT_MIN;
				for (int i = 0; i < echars.Num(); i++)
				{
					float distance = (myLoc - echars[i]->GetActorLocation()).Size();
					if (distance < supportTargetRadiusInPx)
					{
						if (distance > max && target != echars[i])
						{
							target = echars[i];
							max = distance;
						}
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
AGridCharacter* UBlankDecisionComp::FindSupportTarget_LowestStat(AGridCharacter* ignoreThis_)
{
	if (aiManager && ownerEnemy)
	{
		TArray<AEnemyBaseGridCharacter*> echars = aiManager->GetDeployedEnemies();
		FVector myLoc = ownerEnemy->GetActorLocation();
		AEnemyBaseGridCharacter* target = Cast<AEnemyBaseGridCharacter>(ignoreThis_);
		if (echars.Num() > 0)
		{
			float stat_ = 0.0f;
			float min = FLT_MAX;
			if (!bPersistant || !currentTarget) //Only look for a new target if you're not persistent or if you don't have a target
			{
				for (int i = 0; i < echars.Num(); i++)
				{
					if (echars[i])
					{
						float distance = (myLoc - echars[i]->GetActorLocation()).Size();
						if (distance <= supportTargetRadiusInPx) //If they're within our target radius, then check their stat
						{
							stat_ = echars[i]->GetStat(supportConditionStatIndex);
							if (stat_ < min && target != echars[i]) //If they're less than the threshold, then that's our target
							{
								target = echars[i];
								min = stat_;
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
		if (!currentTarget) //Otherwise, find the closest target
		{
			return FindSupportTarget_SmallestDistance(ignoreThis_);
		}
	}
	return nullptr;
}
AGridCharacter* UBlankDecisionComp::FindSupportTarget_HighestStat(AGridCharacter* ignoreThis_)
{
	if (aiManager && ownerEnemy)
	{
		TArray<AEnemyBaseGridCharacter*> echars = aiManager->GetDeployedEnemies();
		FVector myLoc = ownerEnemy->GetActorLocation();
		AEnemyBaseGridCharacter* target = Cast<AEnemyBaseGridCharacter>(ignoreThis_);
		if (echars.Num() > 0)
		{
			float stat_ = 0.0f;
			float max = FLT_MIN;
			if (!bPersistant || !currentTarget) //Only look for a new target if you're not persistent or if you don't have a target
			{
				for (int i = 0; i < echars.Num(); i++)
				{
					if (echars[i])
					{
						float distance = (myLoc - echars[i]->GetActorLocation()).Size();
						if (distance <= supportTargetRadiusInPx) //If they're within our target radius, then check their stat
						{
							stat_ = echars[i]->GetStat(supportConditionStatIndex);
							if (stat_ > max && target != echars[i]) //If they're less than the threshold, then that's our target
							{
								target = echars[i];
								max = stat_;
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
		if (!currentTarget) //Otherwise, find the closest target
		{
			return FindSupportTarget_SmallestDistance(ignoreThis_);
		}
	}
	return nullptr;
}

#pragma endregion

#pragma endregion

#pragma region FindTileFunctions

ATile* UBlankDecisionComp::FindOptimalOffenseTile(ATile* myTile_)
{
	if (currentTarget) 	//Checks if we have a player reference first, otherwise, move to target tile
	{
		ATile* targetTile = ChooseTileBasedOnPossibleOffenseActions(myTile_); //Attacking is the default pattern

		//If all range tiles are occupied, Find another target
		if (!targetTile)
		{
			currentTarget->YouAreNoLongerTargetedByMe(ownerEnemy);
			AGridCharacter* ignoreThisTarget = currentTarget;
			if (bPersistant) //If this enemy is persistent, we need to tell it to find another target and to do that, the current target needs to be made null but we still need to pass it in to avoid it
			{
				currentTarget = nullptr;
			}
			(this->*(findOffenseTarget))(ignoreThisTarget);
			return (this->*(findOffenseTile))(myTile_); //We've switched to a different target so run the function again and find a tile
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
		(this->*(findOffenseTarget))(nullptr);
		return (this->*(findOffenseTile))(myTile_); //We've got a target now so run the function again
	}

	return nullptr;
}
ATile* UBlankDecisionComp::FindOptimalSupportTile(ATile* myTile_)
{
	if (currentTarget) 	//Checks if we have a target reference first, otherwise, move to target tile
	{
		UStatsComponent* statsComp = ownerEnemy->GetStatsComp();
		FindDefensiveSkillThatUpdatesThisStatAndHasTheHighestRange(supportConditionStatIndex, statsComp->GetStatValue(STAT_PIP));
		ATile* targetTile = nullptr;
		if (defenseSkillWithTheMaxRangeIndex != -1)
		{
			targetTile = ChooseTileBasedOnPossibleSupportActions(myTile_); //If we got a skill, then find a possible tile
		}
		else
		{
			//TODO

			//Could not find a usable skill
			return nullptr;
		}
		//If we could not find a feasable tile, then find a different target
		if (!targetTile)
		{
			//	UE_LOG(LogTemp, Warning, TEXT("Tried to heal but couldn't get a tile. Gonna attempt to find another target to heal"));
			currentTarget->YouAreNoLongerTargetedByMe(ownerEnemy);
			AGridCharacter* ignoreThisTarget = currentTarget;
			if (bPersistant) //If this enemy is persistent, we need to tell it to find another target and to do that, the current target needs to be made null but we still need to pass it in to avoid it
			{
				currentTarget = nullptr;
			}
			(this->*(findSupportTarget))(Cast<AEnemyBaseGridCharacter>(ignoreThisTarget));
			return (this->*(findSupportTile))(myTile_); //We've switched to a different target so run the function again and find a tile
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
	else//We don't have a player target, run the functions again
	{
		//	UE_LOG(LogTemp, Warning, TEXT("We don't really have a support target"));					
		(this->*(findSupportTarget))(nullptr);
		return (this->*(findSupportTile))(myTile_); //We've got a target now so run the function again
	}
	return nullptr;
}

#pragma endregion