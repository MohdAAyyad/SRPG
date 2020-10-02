// Fill out your copyright notice in the Description page of Project Settings.


#include "DecisionComp.h"
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
	offenseSkillWithTheMaxRangeIndex = 0;
	defenseSkillWithTheMaxRangeIndex = 0;
	bCanUseSkill = true; //Affected by remaning pips
	bWillUseSkill = false; //True if the enemy is gonna be using a skill
	skillType = -1;
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
	UpdateEnemySkills();
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
AGridCharacter* UDecisionComp::FindTheOptimalTargetCharacter()
{
	//Finds the best target per enum rules
	switch (currentPattern)
	{
	case EPatterns::DEFAULT:
		return FindDefaultTarget();
	case EPatterns::ASSASSIN:
		targetRadius = RAD_ASSA;
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


ATile* UDecisionComp::FindOptimalTargetTile(ATile* myTile_)
{
	//Reset parameters
	bWillUseSkill = false;
	skillType = -1;
	//Finds best action to do then find a tile within range if any exists
	switch (currentPattern)
	{
	case EPatterns::DEFAULT:
		return FindDefaultTile(myTile_);
	case EPatterns::ASSASSIN:
		return FindDefaultTile(myTile_);
	case EPatterns::FOLLOWER:
		return FindDefaultTile(myTile_);
	case EPatterns::PEOPLEPERSON:
		return FindDefaultTile(myTile_);
	case EPatterns::HEALER:
		return FindDefaultTile(myTile_);
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
	return FindTheOptimalTargetCharacter();
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
		currentTarget = target;
		return currentTarget;
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
		currentTarget = target;
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
			float threshold = HP_ASSA;
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


ATile* UDecisionComp::FindDefaultTile(ATile* myTile_)
{
	ATile* targetTile = ChooseTileBasedOnPossibleOffenseActions(myTile_); //Attacking is the default pattern

	//Checks if we have a player reference first, otherwise, move to target tile
	if (currentTarget)
	{
		//If all range tiles are occupied, Find another target
		if (!targetTile)
		{
			FindAnotherTarget(currentTarget);
			return FindDefaultTile(myTile_); //We've switched to a different target so run the function again and find a tile
		}
		else if (targetTile == myTile_) //We're not gonna be moving
		{
			aiManager->FinishedMoving();
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
		return FindDefaultTile(myTile_); //We've got a target now so run the function again
	}


	/*	else //We're not going to move so return myTile_ occupied bool to true
{
	if (myTile_)
		myTile_->SetOccupied(true);
	if (aiManager)
		aiManager->FinishedMoving();
}*/
	
	return nullptr;
}

void UDecisionComp::UpdateEnemySkills()
{
	//Store the enemy skills
	if (ownerEnemy)
	{
		UStatsComponent* statsComp = ownerEnemy->GetStatsComp();
		UExternalFileReader* fileReader = ownerEnemy->GetFileReader();
		if (fileReader && statsComp)
		{
			TArray<FSkillTableStruct*> weaponSkills = fileReader->GetOffesniveSkills(0, statsComp->GetStatValue(STAT_WPI), statsComp->GetStatValue(STAT_WSN), statsComp->GetStatValue(STAT_WSI), statsComp->GetStatValue(STAT_LVL));
			TArray<FSkillTableStruct*> armorSkills = fileReader->GetDefensiveSkills(1, statsComp->GetStatValue(STAT_ARI), statsComp->GetStatValue(STAT_ASN), statsComp->GetStatValue(STAT_ASI), statsComp->GetStatValue(STAT_LVL));
			for (auto w : weaponSkills)
			{
				offsenseSkills.Push(*w);
			}

			for (auto a : armorSkills)
			{
				defenseSkills.Push(*a);
			}
		}

		//Get the skills with the max range
		int maxOfRange = -1;
		for (int i = 0; i < offsenseSkills.Num(); i++)
		{
			if (offsenseSkills[i].rge > maxOfRange)
			{
				offenseSkillWithTheMaxRangeIndex = i;
				maxOfRange = offsenseSkills[i].rge;
			}
		}

		int maxDefRange = -1;
		for (int i = 0; i < defenseSkills.Num(); i++)
		{
			if (defenseSkills[i].rge > maxDefRange)
			{
				defenseSkillWithTheMaxRangeIndex = i;
				maxDefRange = defenseSkills[i].rge;
			}
		}
	}
}

ATile* UDecisionComp::ChooseTileBasedOnPossibleOffenseActions(ATile* myTile_)
{

	//Check skills first. If one of our skills can reach the player, we got the tile
	//If not or if we don't enough pips, then check regular attacks
	//If neither are within range then pick the a tile based on the one with the shortest range

	//Checks the ranges of the skills and the regular attack and sees which is more viable to be used

	AGridManager* gridManager = ownerEnemy->GetGridManager();
	UStatsComponent* statsComp = ownerEnemy->GetStatsComp();
	UPathComponent* pathComp = ownerEnemy->GetPathComponent();
	TArray<ATile*> rangeTiles;
	TArray<ATile*> movementTiles;
	ATile* resultTile = nullptr;

	//Get our movement tiles
	gridManager->UpdateCurrentTile(ownerEnemy->GetOriginTile(), pathComp->GetRowSpeed(), pathComp->GetDepth(), TILE_ENM, -1);
	movementTiles = gridManager->GetHighlightedTiles();
	gridManager->ClearHighlighted();


	if (bCanUseSkill)
	{
		//Check if we're within range of the skill with the longest range first. Reason: Skills will usually have a higher range than regular attacks
		if (statsComp->GetStatValue(STAT_PIP) >= offsenseSkills[offenseSkillWithTheMaxRangeIndex].pip) //Do we have enough pips?
		{
			if (CheckIfPlayerIsInRangeOfSkill(gridManager, pathComp, movementTiles, rangeTiles, &myTile_, &resultTile))
			{
				bWillUseSkill = true;
				skillType = 0;
				return resultTile;
			}
			else
			{
				//If we're not within range of the skill with the longest range, then check if we're in range of a regular attack
				if (CheckIfPlayerIsInRangeOfRegularAttack(gridManager, statsComp, pathComp, movementTiles, rangeTiles, &myTile_, &resultTile))
				{
					return resultTile;
				}
				else
				{
					//If we're in range of neither, then choose a tile based on the one with the least range
					PickAttackOrSkillBasedOnLeastRange(gridManager, statsComp, pathComp, movementTiles, rangeTiles, &myTile_, &resultTile);

					//If we can't find a tile here^ result tile will remain nullptr which will make FindDefaultTile call FindAnotherTarget
				}
			}
		}
		else //Not enough pips to use the skills with the longest range. So look for the next skill with the longest range
		{
			bCanUseSkill = false;
			PickTheNextUsableSkill(statsComp, true);
			ChooseTileBasedOnPossibleOffenseActions(myTile_); //Try again
		}
	}
	else
	{
		//Can't use skills so just use a regular attack
		CheckIfPlayerIsInRangeOfRegularAttack(gridManager, statsComp, pathComp, movementTiles, rangeTiles, &myTile_, &resultTile);
	}

	return resultTile;
}


ATile* UDecisionComp::ChooseTileBasedOnPossibleDefenseActions(ATile* myTile_)
{
	return nullptr;
}



bool UDecisionComp::CheckIfPlayerIsInRangeOfSkill(class AGridManager* grid_, class UPathComponent*path_,
	TArray<ATile*>& movementTiles_, TArray<ATile*>& rangeTiles_, ATile** myTile_, ATile** resultTile_)
{
	//Get the tiles that put the enemy within range of the player.
	grid_->ClearHighlighted();
	grid_->UpdateCurrentTile(currentTarget->GetMyTile(), offsenseSkills[offenseSkillWithTheMaxRangeIndex].rge, offsenseSkills[offenseSkillWithTheMaxRangeIndex].rge + 1, TILE_ENM, offsenseSkills[offenseSkillWithTheMaxRangeIndex].pure);
	rangeTiles_ = grid_->GetHighlightedTiles();
	grid_->ClearHighlighted();

	if (!rangeTiles_.Contains(*myTile_)) //Check if we're not within attacking range
	{
		for (int i = 0; i < rangeTiles_.Num(); i++)
		{
			if (movementTiles_.Contains(rangeTiles_[i]))
			{
				if (!rangeTiles_[i]->GetOccupied())
				{
					*resultTile_ = rangeTiles_[i]; //That's the tile we're going for
					return true;
				}
			}
		}
	}
	else
	{
		//We're not gonna move so the target tile is the same as the tile we're currently on
		*resultTile_ = *myTile_;
		return true;
	}

	*resultTile_ = nullptr;
	return false;
}


bool UDecisionComp::CheckIfPlayerIsInRangeOfRegularAttack(class AGridManager* grid_, UStatsComponent* statsComp_, class UPathComponent*path_,
	TArray<ATile*>& movementTiles_, TArray<ATile*>& rangeTiles_, ATile** myTile_, ATile** resultTile_)
{
	//Check if any of the range tiles is within our movement range
	rangeTiles_.Empty(); //Clear the range tiles
	grid_->UpdateCurrentTile(currentTarget->GetMyTile(), statsComp_->GetStatValue(STAT_WRS), statsComp_->GetStatValue(STAT_WDS), TILE_ENM, statsComp_->GetStatValue(STAT_PURE));
	rangeTiles_ = grid_->GetHighlightedTiles();
	grid_->ClearHighlighted();

	if (!rangeTiles_.Contains(*myTile_)) //Check if we're not within attacking range
	{
		for (int i = 0; i < rangeTiles_.Num(); i++)
		{
			if (movementTiles_.Contains(rangeTiles_[i]))
			{
				if (!rangeTiles_[i]->GetOccupied())
				{
					*resultTile_ = rangeTiles_[i]; //That's the tile we're going for
					return true;
				}
			}
		}
	}
	else
	{
		*resultTile_ = *myTile_;
		return true;
	}
	*resultTile_ = nullptr;
	return false;
}

void UDecisionComp::PickAttackOrSkillBasedOnLeastRange(class AGridManager* grid_, UStatsComponent* statsComp_, class UPathComponent*path_,
	TArray<ATile*>& movementTiles, TArray<ATile*>& rangeTiles_, ATile** myTile_, ATile** resultTile_)
{
	if (statsComp_->GetStatValue(STAT_WRS) <= offsenseSkills[offenseSkillWithTheMaxRangeIndex].rge)
	{
		//Find an non-occupied range tile
		for (int i = rangeTiles_.Num() - 1; i > -1; i--)
		{
			if (rangeTiles_[i]->GetOccupied())
			{
				rangeTiles_.RemoveAt(i);
			}
		}
		if (rangeTiles_.Num() > 0) //If there's a non-occupied range tile then return it
		{
			*resultTile_ = rangeTiles_[rangeTiles_.Num() - 1];
		}
		
	}
	else //Do the same for skills. 
	{
		grid_->ClearHighlighted();
		grid_->UpdateCurrentTile(currentTarget->GetMyTile(), offsenseSkills[offenseSkillWithTheMaxRangeIndex].rge, offsenseSkills[offenseSkillWithTheMaxRangeIndex].rge + 1, TILE_ENM, offsenseSkills[offenseSkillWithTheMaxRangeIndex].pure);
		rangeTiles_ = grid_->GetHighlightedTiles();
		grid_->ClearHighlighted();

		//Find an non-occupied range tile
		for (int i = rangeTiles_.Num() - 1; i > -1; i--)
		{
			if (rangeTiles_[i]->GetOccupied())
			{
				rangeTiles_.RemoveAt(i);
			}
		}
		if (rangeTiles_.Num() > 0) //If there's a non-occupied range tile then return it
		{
			*resultTile_ = rangeTiles_[rangeTiles_.Num() - 1];
			bWillUseSkill = true;
			skillType = 0;
		}
	}
}



void UDecisionComp::PickTheNextUsableSkill(UStatsComponent* statsComp_, bool offense_)
{
	if (offense_)
	{
		int currentIndex = offenseSkillWithTheMaxRangeIndex;
		int newMax = -1;
		int newIndex = -1;
		for (int i = 0; i < offsenseSkills.Num(); i++)
		{
			if (offsenseSkills[i].rge > newMax && i != offenseSkillWithTheMaxRangeIndex)
			{
				if (statsComp_->GetStatValue(STAT_PIP) >= offsenseSkills[i].pip) //Make sure we can use the skill
				{
					newMax = offsenseSkills[i].rge;
					newIndex = i;
					bCanUseSkill = true; //Enters this function as false, and will only become true if we find a suitable skill
				}
			}
		}
		if (bCanUseSkill)
			offenseSkillWithTheMaxRangeIndex = newIndex;
	}
}


bool UDecisionComp::GetWillUseSkill()
{
	return bWillUseSkill;
}
FSkillTableStruct UDecisionComp::GetChosenSkill()
{
	if (skillType == 0)
		return offsenseSkills[offenseSkillWithTheMaxRangeIndex];
	else if (skillType == 1)
		return defenseSkills[defenseSkillWithTheMaxRangeIndex];

	return FSkillTableStruct();
}

/*
Begin play: decision component (DC from here on out) gets all the usable skills from the table 
(same equipment-based rules that apply for the player here) --> DC stores the index of the skill with the 
highest range (SH from here on out) --> Enemy asks  DC to pick a target --> DC follows the rules of its 
pattern type enum to pick a target (DEFAULT, ASSASSIN...etc) --> DC then highlights the movement of the 
enemy-->DC checks if enemy has enough pips to use SH --> Let's assume that the enemy does have enough pips, 
DC then highlights the tiles around the target based on SH's row and depth speeds --> DC checks whether any of the 
range tiles are within movement range. If they are, pick one that's not occupied and that becomes our target tile 
and DC marks a boolean that tells the enemy that we're gonna be using a skill --> If DC does not find a range tile 
within the movement tiles, then it attempts to find a reachable tile based on the regular attack range --> If it 
doesn't find a reachable tile this way either, then the picks either the regular attack or the SH based on which 
has the lower range value (i.e. get as close as possible since you're out of range completely) and based on that 
choice picks a range tile accordingly as the target.


In the case the enemy does not have enough pips, DC looks through the enemy skills and attempts to find the skill 
with the highest range and with a less pip requirement than the current pip value of the enemy. If it does not  find one,
then it goes the regular attack route.

Once a tile is picked, everything is normal from here on out, the enemy's path component finds a path to the tile, 
then the path is adjusted to make sure the end tile is not occupied and the path is completely within movement range. 
When the enemy reaches the final destination, it checks the aforementioned skill boolean and if a skill to be used, it 
sees if its within skill range from the player and attacks them or stays still if out of range.
This is the way most enemies will do it. Enemies with support skills will have a few more decisions to make but I 
haven't thought about it yet
*/

