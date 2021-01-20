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
	offenseTargetRadiusInTiles = 0;
	supportTargetRadiusInTiles = 0;
	functionIndex = 0;
}

void UBlankDecisionComp::BeginPlay()
{
	Super::BeginPlay();
	UpdateFunctionPointers();

	offenseTargetRadiusInPx = static_cast<float>(offenseTargetRadiusInTiles) * TILE_SIZE;
	supportTargetRadiusInPx = static_cast<float>(supportTargetRadiusInTiles) * TILE_SIZE;
}

void UBlankDecisionComp::ResetFunctionIndex() //Called at the beginning of the enemy turn
{
	functionIndex = 0;
}
void UBlankDecisionComp::UpdateFunctionPointers()
{
	//Loop through the conditions array and populate the function pointer arrays accordingly
	for (int i = 0; i < conditions.Num(); i++)
	{
		findTargetFuncPtr  findTarget;
		findTileFuncPtr findTile;
		switch (conditions[i])
		{
		case ETargetConditions::OFFENSE_SMALLEST_DISTANCE:
			findTarget = &UBlankDecisionComp::FindOffenseTarget_SmallestDistance;
			findTile = &UBlankDecisionComp::FindOptimalOffenseTile;
			break;
		case ETargetConditions::OFFENSE_LONGEST_DISTANCE:
			findTarget = &UBlankDecisionComp::FindOffenseTarget_LongestDistance;
			findTile = &UBlankDecisionComp::FindOptimalOffenseTile;
			break;
		case ETargetConditions::OFFENSE_LOWEST_CONDITIONED_STAT:
			findTarget = &UBlankDecisionComp::FindOffenseTarget_LowestStat;
			findTile = &UBlankDecisionComp::FindOptimalOffenseTile;
			break;
		case ETargetConditions::OFFENSE_HIGHEST_CONDITIONED_STAT:
			findTarget = &UBlankDecisionComp::FindOffenseTarget_HighestStat;
			findTile = &UBlankDecisionComp::FindOptimalOffenseTile;
			break;


		case ETargetConditions::SUPPORT_SMALLEST_DISTANCE: //Picks a target based on distance THEN decides which skill to choose then picks a tile
 			findTarget = &UBlankDecisionComp::FindSupportTarget_SmallestDistance;
			findTile = &UBlankDecisionComp::FindOptimalDistanceBasedSupportTile;
			break;
		case ETargetConditions::SUPPORT_LONGEST_DISTANCE:
			findTarget = &UBlankDecisionComp::FindSupportTarget_LongestDistance;
			findTile = &UBlankDecisionComp::FindOptimalDistanceBasedSupportTile;
			break;
		case ETargetConditions::SUPPORT_LOWEST_CONDITIONED_STAT: //Chooses a skil to use first then picks a tile.
			findTarget = &UBlankDecisionComp::FindSupportTarget_LowestStat;
			findTile = &UBlankDecisionComp::FindOptimalStatBasedSupportTile;
			break;
		case ETargetConditions::SUPPORT_HIGHEST_CONDITIONED_STAT:
			findTarget = &UBlankDecisionComp::FindSupportTarget_HighestStat;
			findTile = &UBlankDecisionComp::FindOptimalStatBasedSupportTile;
			break;
		default:
			break;
		}
		targetFunctionPtrs.Push(findTarget);
		tileFunctionPtrs.Push(findTile);
	}
}

AGridCharacter* UBlankDecisionComp::FindTheOptimalTargetCharacter()
{
	bWillUseSkill = false;
	skillType = -1;

	if (functionIndex >= 0 && functionIndex < targetFunctionPtrs.Num())
	{
		return (this->*(targetFunctionPtrs[functionIndex]))(nullptr);
	}

	return nullptr;
}
ATile* UBlankDecisionComp::FindOptimalTargetTile(ATile* myTile_)
{
	if (functionIndex >= 0 && functionIndex < tileFunctionPtrs.Num())
	{
		return (this->*(tileFunctionPtrs[functionIndex]))(myTile_);
	}
	return nullptr;
}

//Used for changing targets after failing to find the optimal one
AGridCharacter* UBlankDecisionComp::FindNextOptimalTargetCharacter(AGridCharacter* ignoreThis_)
{
	//UE_LOG(LogTemp, Warning, TEXT("Find next"));
	bWillUseSkill = false;
	skillType = -1;

	if (currentTarget)
		currentTarget->YouAreNoLongerTargetedByMe(ownerEnemy);

	currentTarget = nullptr; //We're switching targets

	if (ownerEnemy)
		ownerEnemy->ResetActionTargets(); //Clear the targets array

	if (functionIndex >= 0 && functionIndex < targetFunctionPtrs.Num())
	{
		return (this->*(targetFunctionPtrs[functionIndex]))(ignoreThis_);
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
						//UE_LOG(LogTemp, Warning, TEXT("Blank: Found potential target"));
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
		{
			
			//UE_LOG(LogTemp, Warning, TEXT("CURRENT TARGETTTT %s"), *currentTarget->GetName());
			currentTarget->YouAreTargetedByMeNow(ownerEnemy);
			return currentTarget;
		}
		else
		{
			//if you haven't found a suitable target, move on to the next function
			functionIndex++;
			if (functionIndex >= targetFunctionPtrs.Num())
			{
				functionIndex = 0;
				return nullptr; //You've exhausted all your options.
			}
			else
			{
				return FindNextOptimalTargetCharacter(ignoreThis_);
			}	
			
		}
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
		{
			currentTarget->YouAreTargetedByMeNow(ownerEnemy);
			return currentTarget;
		}
		else
		{
			//if you haven't found a suitable target, move on to the next function
			functionIndex++;
			if (functionIndex >= targetFunctionPtrs.Num())
			{
				functionIndex = 0;
				return nullptr; //You've exhausted all your options.
			}
			else
			{
				return FindNextOptimalTargetCharacter(ignoreThis_);
			}

		}
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
				currentTarget = target;
			}
		}

		if (currentTarget)
		{
			currentTarget->YouAreTargetedByMeNow(ownerEnemy);
			return currentTarget;
		}
		else
		{
			//if you haven't found a suitable target, move on to the next function
			functionIndex++;
			if (functionIndex >= targetFunctionPtrs.Num())
			{
				functionIndex = 0;
				return nullptr; //You've exhausted all your options.
			}
			else
			{
				return FindNextOptimalTargetCharacter(ignoreThis_);
			}

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

				currentTarget = target;
			}
		}

		if (currentTarget)
		{
			currentTarget->YouAreTargetedByMeNow(ownerEnemy);
			return currentTarget;
		}
		else
		{
			//if you haven't found a suitable target, move on to the next function
			functionIndex++;
			if (functionIndex >= targetFunctionPtrs.Num())
			{
				functionIndex = 0;
				return nullptr; //You've exhausted all your options.
			}
			else
			{
				return FindNextOptimalTargetCharacter(ignoreThis_);
			}

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
					if (echars[i] != ownerEnemy) //Can't choose self
					{
						float distance = (myLoc - echars[i]->GetActorLocation()).Size();
						if (distance <= supportTargetRadiusInPx)
						{
							if (distance < min && target != echars[i])
							{
								target = echars[i];
								min = distance;
							}
						}
					}
				}
			}
			currentTarget = target;
		}

		if (currentTarget)
		{
			currentTarget->YouAreTargetedByMeNow(ownerEnemy);
			return currentTarget;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Called next from FindSupportTarget_SmallestDistance"));
			//if you haven't found a suitable target, move on to the next function
			functionIndex++;
			if (functionIndex >= targetFunctionPtrs.Num())
			{
				functionIndex = 0;
				return nullptr; //You've exhausted all your options.
			}
			else
			{
				return FindNextOptimalTargetCharacter(ignoreThis_);
			}

		}
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
					if (echars[i] != ownerEnemy) //Can't choose self
					{
						float distance = (myLoc - echars[i]->GetActorLocation()).Size();
						if (distance <= supportTargetRadiusInPx)
						{
							if (distance > max && target != echars[i])
							{
								target = echars[i];
								max = distance;
							}
						}
					}
				}
			}
			currentTarget = target;
		}

		if (currentTarget)
		{
			currentTarget->YouAreTargetedByMeNow(ownerEnemy);
			return currentTarget;
		}
		else
		{
			//if you haven't found a suitable target, move on to the next function
			functionIndex++;
			if (functionIndex >= targetFunctionPtrs.Num())
			{
				functionIndex = 0;
				return nullptr; //You've exhausted all your options.
			}
			else
			{
				return FindNextOptimalTargetCharacter(ignoreThis_);
			}

		}
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
		UStatsComponent* statsComp = ownerEnemy->GetStatsComp();
		if (echars.Num() > 0)
		{
			float stat_ = 0.0f;
			float distance = 0.0f;
			float min = FLT_MAX;
			if (!bPersistant || !currentTarget) //Only look for a new target if you're not persistent or if you don't have a target
			{
				for (int j = 0; j < supportPriorityConditionStatIndex.Num(); j++)
				{
					defenseSkillWithTheMaxRangeIndex = -1;
					FindDefensiveSkillThatUpdatesThisStatAndHasTheHighestRange(supportPriorityConditionStatIndex[j], statsComp->GetStatValue(STAT_PIP));
					if (defenseSkillWithTheMaxRangeIndex != -1) //Only check for the validty of the target if we have a skill that affects the stat in question
					{
						for (int i = 0; i < echars.Num(); i++)
						{
							if (echars[i])
							{
								distance = (myLoc - echars[i]->GetActorLocation()).Size();
								stat_ = echars[i]->GetStatsComp()->GetStatValue(supportPriorityConditionStatIndex[j]);

								if (distance <= supportTargetRadiusInPx) //If they're within our target radius, then check their stat
								{
									if (supportPriorityConditionStatIndex[j] == STAT_HP) //If the stat we're checking for is HP then make sure the target has less hp than the threshold
									{
										if (echars[i]->GetStatsComp()->GetHPPercentage() <= hpThresholdForSupport)
										{
											if (stat_ < min && target != echars[i]) //Now check for min
											{
												target = echars[i];
												min = stat_;
											}
										}
									}
									else if (supportPriorityConditionStatIndex[j] == STAT_PIP) //If the stat we're checking for is PIP then make sure the target has less pip than the threshold
									{
										if (echars[i]->GetStatsComp()->GetPIPPercentage() <= pipThresholdForSupport)
										{
											if (stat_ < min && target != echars[i]) //Now check for min
											{
												target = echars[i];
												min = stat_;
											}
										}
									}
									else //It's not hp or pip so just make sure the character was not buffed for that particular stat
									{
										if (!echars[i]->GetStatsComp()->HasThisStatBeenBuffed(supportPriorityConditionStatIndex[j]))
										{
											if (stat_ < min && target != echars[i]) //Now check for min
											{
												target = echars[i];
												min = stat_;
											}
										}
									}

								}
							}
						}

						if (target != nullptr && target != ignoreThis_) //We've found a target for the skill, leave
						{
							break;
						}
					}
				}

				currentTarget = target;
			}
		}

		if (currentTarget)
		{
			currentTarget->YouAreTargetedByMeNow(ownerEnemy);
			return currentTarget;
		}
		else
		{
			//if you haven't found a suitable target, move on to the next function
			functionIndex++;
			if (functionIndex >= targetFunctionPtrs.Num())
			{
				functionIndex = 0;
				return nullptr; //You've exhausted all your options.
			}
			else
			{
				return FindNextOptimalTargetCharacter(ignoreThis_);
			}

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
		UStatsComponent* statsComp = ownerEnemy->GetStatsComp();
		if (echars.Num() > 0)
		{
			float distance = 0.0f;
			float stat_ = 0.0f;
			float max = FLT_MIN;
			if (!bPersistant || !currentTarget) //Only look for a new target if you're not persistent or if you don't have a target
			{
				for (int j = 0; j < supportPriorityConditionStatIndex.Num(); j++)
				{
					defenseSkillWithTheMaxRangeIndex = -1;
					FindDefensiveSkillThatUpdatesThisStatAndHasTheHighestRange(supportPriorityConditionStatIndex[j], statsComp->GetStatValue(STAT_PIP));
					if (defenseSkillWithTheMaxRangeIndex != -1) //Only check for the validty of the target if we have a skill that affects the stat in question
					{
						for (int i = 0; i < echars.Num(); i++)
						{
							if (echars[i])
							{
								distance = (myLoc - echars[i]->GetActorLocation()).Size();
								stat_ = echars[i]->GetStatsComp()->GetStatValue(supportPriorityConditionStatIndex[j]);

								if (distance <= supportTargetRadiusInPx) //If they're within our target radius, then check their stat
								{
									if (supportPriorityConditionStatIndex[j] == STAT_HP) //If the stat we're checking for is HP then make sure the target has less hp than the threshold
									{
										if (echars[i]->GetStatsComp()->GetHPPercentage() <= hpThresholdForSupport)
										{
											if (stat_ > max && target != echars[i]) //Now check for max
											{
												target = echars[i];
												max = stat_;
											}
										}
									}
									else if (supportPriorityConditionStatIndex[j] == STAT_PIP) //If the stat we're checking for is PIP then make sure the target has less pip than the threshold
									{
										if (echars[i]->GetStatsComp()->GetPIPPercentage() <= pipThresholdForSupport)
										{
											if (stat_ > max && target != echars[i]) //Now check for max
											{
												target = echars[i];
												max = stat_;
											}
										}
									}
									else //It's not hp or pip so just make sure the character was not buffed for that particular stat
									{
										if (!echars[i]->GetStatsComp()->HasThisStatBeenBuffed(supportPriorityConditionStatIndex[j]))
										{
											if (stat_ > max && target != echars[i]) //Now check for max
											{
												target = echars[i];
												max = stat_;
											}
										}
									}

								}
							}
						}

						if (target != nullptr && target != ignoreThis_) //We've found a target for the skill, leave
						{
							break;
						}
					}
				}

				currentTarget = target;
			}
		}

		if (currentTarget)
		{
			currentTarget->YouAreTargetedByMeNow(ownerEnemy);
			return currentTarget;
		}
		else
		{
			//if you haven't found a suitable target, move on to the next function
			functionIndex++;
			if (functionIndex >= targetFunctionPtrs.Num())
			{
				functionIndex = 0;
				return nullptr; //You've exhausted all your options.
			}
			else
			{
				return FindNextOptimalTargetCharacter(ignoreThis_);
			}

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
			UE_LOG(LogTemp, Warning, TEXT("Could not find target tile"));
			currentTarget->YouAreNoLongerTargetedByMe(ownerEnemy);
			AGridCharacter* ignoreThisTarget = currentTarget;
			if (bPersistant) //If this enemy is persistent, we need to tell it to find another target and to do that, the current target needs to be made null but we still need to pass it in to avoid it
			{
				currentTarget = nullptr;
			}
			//Increment the function index and check if you're out of bounds
			functionIndex++;
			if (functionIndex >= targetFunctionPtrs.Num())
				functionIndex = 0;

			ownerEnemy->UpdateTargetCharacter(FindNextOptimalTargetCharacter(ignoreThisTarget));
			return (FindOptimalTargetTile(myTile_)); //We've switched to a different target so run the function again and find a tile
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
		//Increment the function index and check if you're out of bounds
		functionIndex++;
		if (functionIndex >= targetFunctionPtrs.Num())
		{
			functionIndex = 0; //You've exhausted all options
			return nullptr;
		}
		else
		{

			ownerEnemy->UpdateTargetCharacter(FindNextOptimalTargetCharacter(nullptr));
			return (FindOptimalTargetTile(myTile_)); //We've switched to a different target so run the function again and find a tile
		}
	}

	return nullptr;
}
ATile* UBlankDecisionComp::FindOptimalDistanceBasedSupportTile(ATile* myTile_)
{
	//We have picked a target based on distance as that's the main condition
	//Now we need to make sure we can support this character based on the support priority array

	if (currentTarget) 	//Checks if we have a target reference first, otherwise, move to target tile
	{
		UStatsComponent* statsComp = ownerEnemy->GetStatsComp();
		UStatsComponent* currentTargetStatComp = currentTarget->GetStatsComp();
		defenseSkillWithTheMaxRangeIndex = -1;
		
		//Loop through the priority stat array to find a usable skill
		for (int i = 0; i < supportPriorityConditionStatIndex.Num(); i++)
		{
			//If the stat in question is HP or PIP, then check the threshold first before committing
			if (supportPriorityConditionStatIndex[i] == STAT_HP)
			{
				if (currentTargetStatComp->GetHPPercentage() <= hpThresholdForSupport)
				{
					FindDefensiveSkillThatUpdatesThisStatAndHasTheHighestRange(supportPriorityConditionStatIndex[i], statsComp->GetStatValue(STAT_PIP));
					if (defenseSkillWithTheMaxRangeIndex != -1) //Found a skill based on the priority array, break
						break;
				}
			}
			else if (supportPriorityConditionStatIndex[i] == STAT_PIP)
			{
				if (currentTargetStatComp->GetPIPPercentage() <= pipThresholdForSupport)
				{
					FindDefensiveSkillThatUpdatesThisStatAndHasTheHighestRange(supportPriorityConditionStatIndex[i], statsComp->GetStatValue(STAT_PIP));
					if (defenseSkillWithTheMaxRangeIndex != -1) //Found a skill based on the priority array, break
						break;
				}
			}
			else //It's not hp or pip so just make sure the character was not buffed for that particular stat
			{
				if (!currentTargetStatComp->HasThisStatBeenBuffed(supportPriorityConditionStatIndex[i]))
				{
					FindDefensiveSkillThatUpdatesThisStatAndHasTheHighestRange(supportPriorityConditionStatIndex[i], statsComp->GetStatValue(STAT_PIP));
					if (defenseSkillWithTheMaxRangeIndex != -1) //Found a skill based on the priority array, break
						break;
				}
			}
		}
		
		ATile* targetTile = nullptr;
		if (defenseSkillWithTheMaxRangeIndex != -1)
		{
			//UE_LOG(LogTemp, Warning, TEXT("defenseSkillWithTheMaxRangeIndex != -1 %d"), defenseSkillWithTheMaxRangeIndex);
			targetTile = ChooseTileBasedOnPossibleSupportActions(myTile_); //If we got a skill, then find a possible tile
		}
		else
		{
		
			//Could not find a usable skill so check what the next rule is

			//Increment the function index and check if you're out of bounds
			functionIndex++;
			if (functionIndex >= targetFunctionPtrs.Num())
			{
				functionIndex = 0; //You've exhausted all options
				return nullptr;
			}
			else
			{

				ownerEnemy->UpdateTargetCharacter(FindNextOptimalTargetCharacter(nullptr));
				return (FindOptimalTargetTile(myTile_)); //We've switched to a different target so run the function again and find a tile
			}
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

			//UE_LOG(LogTemp, Warning, TEXT("BLANK : Could not find a support target tile"));
			//Increment the function index and check if you're out of bounds
			functionIndex++;
			if (functionIndex >= targetFunctionPtrs.Num())
			{
				functionIndex = 0; //You've exhausted all options
				return nullptr;
			}
			else
			{

				ownerEnemy->UpdateTargetCharacter(FindNextOptimalTargetCharacter(ignoreThisTarget));
				return (FindOptimalTargetTile(myTile_)); //We've switched to a different target so run the function again and find a tile
			}
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
		//UE_LOG(LogTemp, Warning, TEXT("BLANK : Don't have a support target"));
		//Increment the function index and check if you're out of bounds
		functionIndex++;
		if (functionIndex >= targetFunctionPtrs.Num())
		{
			functionIndex = 0; //You've exhausted all options
			return nullptr;
		}
		else
		{

			ownerEnemy->UpdateTargetCharacter(FindNextOptimalTargetCharacter(nullptr));
			return (FindOptimalTargetTile(myTile_)); //We've switched to a different target so run the function again and find a tile
		}
	}
	return nullptr;
}

ATile* UBlankDecisionComp::FindOptimalStatBasedSupportTile(ATile* myTile_)
{
	//If we're here, that means we've already found a skill to use and found a valid target

	if (currentTarget)
	{
		ATile* targetTile = nullptr;
		if (defenseSkillWithTheMaxRangeIndex != -1) //Extra precaution
		{
			//UE_LOG(LogTemp, Warning, TEXT("defenseSkillWithTheMaxRangeIndex != -1 %d"), defenseSkillWithTheMaxRangeIndex);
			targetTile = ChooseTileBasedOnPossibleSupportActions(myTile_); //If we got a skill, then find a possible tile
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

			//UE_LOG(LogTemp, Warning, TEXT("BLANK : Could not find a support target tile"));
			//Increment the function index and check if you're out of bounds
			functionIndex++;
			if (functionIndex >= targetFunctionPtrs.Num())
			{
				functionIndex = 0; //You've exhausted all options
				return nullptr;
			}
			else
			{

				ownerEnemy->UpdateTargetCharacter(FindNextOptimalTargetCharacter(ignoreThisTarget));
				return (FindOptimalTargetTile(myTile_)); //We've switched to a different target so run the function again and find a tile
			}
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
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("BLANK : Don't have a support target"));
				//Increment the function index and check if you're out of bounds
		functionIndex++;
		if (functionIndex >= targetFunctionPtrs.Num())
		{
			functionIndex = 0; //You've exhausted all options
			return nullptr;
		}
		else
		{

			ownerEnemy->UpdateTargetCharacter(FindNextOptimalTargetCharacter(nullptr));
			return (FindOptimalTargetTile(myTile_)); //We've switched to a different target so run the function again and find a tile
		}
	}

	return nullptr;
}

#pragma endregion