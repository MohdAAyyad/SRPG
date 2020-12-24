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
#include "../Crowd/CrowdItem.h"

// Sets default values for this component's properties
UDecisionComp::UDecisionComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bTargetIsHostile = true;
	aiManager = nullptr;
	btlManager = nullptr;
	ownerEnemy = nullptr;
	offenseSkillWithTheMaxRangeIndex = -1;
	defenseSkillWithTheMaxRangeIndex = -1;
	bCanUseSkill = true; //Affected by remaning pips
	bWillUseSkill = false; //True if the enemy is gonna be using a skill
	skillType = -1; //-1 if enemy will not use a skill, 0 if offense skill, and 1 if defense skill
	skillChance = 40;
	bPersistant = false;
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
	if (ownerEnemy->GetStatsComp()->GetStatValue(STAT_ARCH) == ARCH_INT)
	{
		skillChance += 30;
	}
	else if (ownerEnemy->GetStatsComp()->GetStatValue(STAT_ARCH) == ARCH_DEF)
	{
		skillChance += 10;
	}
	UpdateEnemySkills();
}
AGridCharacter* UDecisionComp::FindTheOptimalTargetCharacter()
{
	return nullptr;
}


ATile* UDecisionComp::FindOptimalTargetTile(ATile* myTile_)
{
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
	UE_LOG(LogTemp, Warning, TEXT("Choose tile based on possible offesen actions"));

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

	UE_LOG(LogTemp, Warning, TEXT("offsenseSkills.Num() %d"), offsenseSkills.Num());
	UE_LOG(LogTemp, Warning, TEXT("statsComp->GetStatValue(STAT_PIP) %d"), statsComp->GetStatValue(STAT_PIP));
	UE_LOG(LogTemp, Warning, TEXT("offsenseSkills[offenseSkillWithTheMaxRangeIndex].pip %d"), offsenseSkills[offenseSkillWithTheMaxRangeIndex].pip);

	UE_LOG(LogTemp, Warning, TEXT("bCanUseSkill %d"), bCanUseSkill);

	if (offsenseSkills.Num() > 0)
	{
		if (bCanUseSkill)
		{
			//Check if we're within range of the skill with the longest range first. Reason: Skills will usually have a higher range than regular attacks
			if (statsComp->GetStatValue(STAT_PIP) >= offsenseSkills[offenseSkillWithTheMaxRangeIndex].pip) //Do we have enough pips?
			{
				//If we have enough pips, roll a dice and see if we get to use a skill
				if (skillChance >= FMath::RandRange(0, 100))
				{
					UE_LOG(LogTemp, Warning, TEXT("Bypassed skill chance check"));
					if (CheckIfTargetIsInRangeOfSkill(gridManager, pathComp, movementTiles, rangeTiles, &myTile_, &resultTile,true))
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
				else //Chose to do a regular attack
				{
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
	else
	{
		//Don't have any offensive skills so just use a regular attack
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

	return resultTile;
}

ATile* UDecisionComp::ChooseTileBasedOnPossibleSupportActions(ATile* myTile_)
{
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

	if (CheckIfTargetIsInRangeOfSkill(gridManager, pathComp, movementTiles, rangeTiles, &myTile_, &resultTile,false))
	{
		//UE_LOG(LogTemp, Warning, TEXT("The target is within the support skill we've checked!"));
		bWillUseSkill = true;
		skillType = 1;
		return resultTile;
	}
	return nullptr;
}


void UDecisionComp::FindDefensiveSkillThatUpdatesThisStatAndHasTheHighestRange(int statIndex_, int currentPips_)
{
	int maxRange = FLT_MIN;
	UE_LOG(LogTemp, Warning, TEXT("statindex_ %d"), statIndex_);
	for (int i = 0; i < defenseSkills.Num(); i++)
	{
		//Get the skill that matches the stat we're looking for, has the highest range, and usable
		if (defenseSkills[i].statIndex == statIndex_ && defenseSkills[i].rge > maxRange && defenseSkills[i].pip <= currentPips_)
		{
			UE_LOG(LogTemp, Warning, TEXT("defenseSkills[i].statIndex %d"), defenseSkills[i].statIndex);
			defenseSkillWithTheMaxRangeIndex = i;
		}
	}
}

bool UDecisionComp::CheckIfTargetIsInRangeOfSkill(AGridManager* grid_, class UPathComponent*path_,
	TArray<ATile*>& movementTiles_, TArray<ATile*>& rangeTiles_, ATile** myTile_, ATile** resultTile_, bool offense_)
{
	grid_->ClearHighlighted();
	if (currentTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got a current target inside CheckIfTargetIsInRangeOfSkill "));
		if (offense_)
		{
			grid_->UpdateCurrentTile(currentTarget->GetMyTile(), offsenseSkills[offenseSkillWithTheMaxRangeIndex].rge, offsenseSkills[offenseSkillWithTheMaxRangeIndex].rge + 1, TILE_ENMA, offsenseSkills[offenseSkillWithTheMaxRangeIndex].pure);
		}
		else
		{
			grid_->UpdateCurrentTile(currentTarget->GetMyTile(), defenseSkills[defenseSkillWithTheMaxRangeIndex].rge, defenseSkills[defenseSkillWithTheMaxRangeIndex].rge + 1, TILE_ENMA, defenseSkills[defenseSkillWithTheMaxRangeIndex].pure);
		}
		//Get the tiles that put the enemy within range of the target.
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
	}

	*resultTile_ = nullptr;
	return false;
}


bool UDecisionComp::CheckIfPlayerIsInRangeOfRegularAttack(class AGridManager* grid_, UStatsComponent* statsComp_, class UPathComponent*path_,
	TArray<ATile*>& movementTiles_, TArray<ATile*>& rangeTiles_, ATile** myTile_, ATile** resultTile_)
{
	//Check if any of the range tiles is within our movement range
	rangeTiles_.Empty(); //Clear the range tiles
	grid_->UpdateCurrentTile(currentTarget->GetMyTile(), statsComp_->GetStatValue(STAT_WRS), statsComp_->GetStatValue(STAT_WDS), TILE_ENMA, statsComp_->GetStatValue(STAT_PURE));
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
	UE_LOG(LogTemp, Warning, TEXT("Picking attack based on least range"));
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
			UE_LOG(LogTemp, Warning, TEXT("Yeah went with regular attack"));
			//Reset skills first
			*resultTile_ = rangeTiles_[rangeTiles_.Num() - 1];
		}
		
	}
	else //Do the same for skills. 
	{
		grid_->ClearHighlighted();
		grid_->UpdateCurrentTile(currentTarget->GetMyTile(), offsenseSkills[offenseSkillWithTheMaxRangeIndex].rge, offsenseSkills[offenseSkillWithTheMaxRangeIndex].rge + 1, TILE_ENMA, offsenseSkills[offenseSkillWithTheMaxRangeIndex].pure);
		rangeTiles_ = grid_->GetHighlightedTiles();
		grid_->ClearHighlighted();

		//Find a non-occupied range tile
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

	if(*resultTile_)
		UE_LOG(LogTemp, Warning, TEXT("Got the tile based on least range"));
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

#pragma region GetFunctions

bool UDecisionComp::GetWillUseSkill(int& skillType_)
{
	skillType_ = skillType;
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

void UDecisionComp::ResetCurrentTarget()
{
	//Called when our main target is dead
	currentTarget = nullptr;
}

#pragma endregion

#pragma region Items
ACrowdItem* UDecisionComp::UpdateTargetItem(AGridManager* grid_, ATile* originTile_, int rows_, int depths_)
{
	if (ownerEnemy)
	{
		if (grid_)
		{
			if (originTile_)
			{
				grid_->ClearHighlighted();
				grid_->UpdateCurrentTile(originTile_, rows_, depths_, TILE_ENM, 0);

				UStatsComponent* statsComp = ownerEnemy->GetStatsComp();

				for (int i = 0; i < crdItems.Num(); i++)
				{
					if (crdItems[i])
					{
						//Check if we need the item first before marking it
						if (CheckIfINeedThisItem(statsComp,crdItems[i]->GetStat(), crdItems[i]->GetValue()))
						{
							//If we have detected an item, make sure it's within movement range before marking it
							ATile* tile_ = crdItems[i]->GetMyTile();
								if (tile_)
								{
									if (tile_->GetHighlighted() == TILE_ENM)
									{
										if (crdItems[i]->MarkItem(ownerEnemy)) //True, means we've marked this items as our target and no other enemy should go for it
										{
											return crdItems[i];
										}
									}
								}
						}

					}
				}
			}
		}
	}

	return nullptr;
}
void UDecisionComp::AddCrdItem(ACrowdItem* newItem_)
{
	crdItems.Push(newItem_);
}
void UDecisionComp::RemoveCrdItem(ACrowdItem* newItem_)
{
	if (crdItems.Num() > 0)
	{
		if (crdItems.Contains(newItem_))
		{
			crdItems.Remove(newItem_);
		}
	}
}
void UDecisionComp::ClearCrdItems()
{
	if (crdItems.Num() > 0)
	{
		crdItems.Empty();
	}
}

bool UDecisionComp::CheckIfINeedThisItem(UStatsComponent* stats_, int statIndex_, int itemValue_)
{
	if (stats_)
	{
		//If the item restores HP or PIPs and the enemy's current HP or PIP is less than max then you want to take the item
		if (statIndex_ == STAT_HP)
		{
			if (stats_->GetStatValue(statIndex_) < stats_->GetMaxHP())
			{
				return true;
			}
		}
		else if (statIndex_ == STAT_PIP)
		{
			if (stats_->GetStatValue(statIndex_) < stats_->GetMaxPIP())
			{
				return true;
			}
		}
		//Otherwise, only take the item if you haven't been buffed
		else
		{
			if (!stats_->HasThisStatBeenBuffed(statIndex_))
				return true;
		}
	}
	return false;
}
#pragma endregion


void UDecisionComp::SetPersistence(bool value_)
{
	bPersistant = value_;
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

