// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBaseGridCharacter.h"
#include "AIManager.h"
#include "../Player/PlayerGridCharacter.h"
#include "../Grid/Tile.h"
#include "../Grid/GridManager.h"
#include "../PathComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "../BattleController.h"
#include "Definitions.h"
#include "../Crowd/BattleCrowd.h"
#include "../BattleManager.h"
#include "../Crowd/CrowdItem.h"
#include "Components/CapsuleComponent.h"
#include "Definitions.h"
#include "../StatsComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Intermediary/Intermediate.h"


AEnemyBaseGridCharacter::AEnemyBaseGridCharacter() :AGridCharacter()
{
	detectionRadius = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionRadius"));
	detectionRadius->SetupAttachment(RootComponent);
	detectionRadius->SetGenerateOverlapEvents(true);
	detectionRadius->SetCollisionProfileName("EnemyDetectionRadius");
	detectionRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	targetPlayer = nullptr;
	targetTile = nullptr;
	targetItem = nullptr;
	bWillMoveAgain = true;
	bCannotFindTile = false;
	weaponIndex = 0;
	armorIndex = 0;
}

void AEnemyBaseGridCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (detectionRadius)
		detectionRadius->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBaseGridCharacter::DetectItem);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBaseGridCharacter::TakeItem);

	btlCtrl = Cast<ABattleController>(GetWorld()->GetFirstPlayerController());

}

void AEnemyBaseGridCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCannotFindTile) //Necessary in case the enemy needed a new destiantion while transitioning between tiles
	{
		ATile* myTile_ = GetMyTile();

		if (myTile_)
		{
			bCannotFindTile = false;
			bMoving = false;
			MoveCloserToTargetPlayer(myTile_);
		}
	}
}
void AEnemyBaseGridCharacter::SetManagers(AAIManager* ref_, AGridManager* gref_, ABattleManager* bref_)
{
	//Called by AI Manager on Beginplay
	aiManager = ref_;
	gridManager = gref_;
	btlManager = bref_;

	statsComp->InitStatsAtZero();
	statsComp->AddToStat(STAT_WPI, weaponIndex);
	statsComp->AddToStat(STAT_ARI, armorIndex);
	statsComp->ScaleLevelWithArchetype(Intermediate::GetInstance()->GetNextOpponent().level, Intermediate::GetInstance()->GetNextOpponent().archtype);
	AddEquipmentStats(2);
}

void AEnemyBaseGridCharacter::AddEquipmentStats(int tableIndex_)
{

	//Get the best equipment you can equip from the table

	FEquipmentTableStruct weapon;
	FEquipmentTableStruct armor;
	FEquipmentTableStruct accessory;

	//Get the stats of the equipment and add them to the character's stats
	if (fileReader)
	{
		weapon = fileReader->GetEquipmentByLevel(tableIndex_, statsComp->GetStatValue(STAT_LVL),EQU_WPN, statsComp->GetStatValue(STAT_WPI));
		armor = fileReader->GetEquipmentByLevel(tableIndex_, statsComp->GetStatValue(STAT_LVL),EQU_ARM, statsComp->GetStatValue(STAT_ARI));
		accessory = fileReader->GetEquipmentByLevel(tableIndex_, statsComp->GetStatValue(STAT_LVL),EQU_ACC,-1);
		


		statsComp->AddToStat(STAT_HP, weapon.hp + armor.hp + accessory.hp);
		statsComp->AddToStat(STAT_PIP, weapon.pip + armor.pip + accessory.pip);
		statsComp->AddToStat(STAT_ATK, weapon.atk + armor.atk + accessory.atk);
		statsComp->AddToStat(STAT_DEF, weapon.def + armor.def + accessory.def);
		statsComp->AddToStat(STAT_INT, weapon.intl + armor.intl + accessory.intl);
		statsComp->AddToStat(STAT_SPD, weapon.spd + armor.spd + accessory.spd);
		statsComp->AddToStat(STAT_CRT, weapon.crit + armor.crit + accessory.crit);
		statsComp->AddToStat(STAT_CRD, weapon.crd + armor.crd + accessory.crd);
		statsComp->AddToStat(STAT_WSI, weapon.skillsIndex);
		statsComp->AddToStat(STAT_WSN, weapon.skillsN);
		statsComp->AddToStat(STAT_ASI, armor.skillsIndex);
		statsComp->AddToStat(STAT_ASN, armor.skillsN);
		statsComp->AddToStat(STAT_WRS, weapon.range);
		statsComp->AddToStat(STAT_WDS, weapon.range + 1);
		statsComp->AddToStat(STAT_PURE, weapon.pure);
	}

	if (pathComp)
		pathComp->UpdateSpeed(statsComp->GetStatValue(STAT_SPD));
}

void AEnemyBaseGridCharacter::MoveCloserToTargetPlayer(ATile* startingTile_)
{
	ATile* myTile_ = startingTile_;
	if (!myTile_)
		myTile_ = GetMyTile();
	bool bGoingToMove = false;

	if (myTile_)
	{
		//Highlight your movement tiles
		if (gridManager && pathComp)
		{
			gridManager->ClearHighlighted();
			gridManager->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(), TILE_ENM,0); //4 Reachable by enemy
			//Checks if we have a player reference first, otherwise, move to target tile
			if (targetPlayer)
			{
				TArray<ATile*> rangeTiles;
				//Get the tiles that put the enemy within range of the player.
				rangeTiles = gridManager->GetTilesWithinAttackRange(statsComp->GetStatValue(STAT_WRS), targetPlayer->GetMyTile());
				if (!rangeTiles.Contains(myTile_)) //Check if we're not within attacking range
				{
					if (myTile_)
						myTile_->SetOccupied(false);
					targetTile = nullptr;
					bGoingToMove = true;

					//Remove the occupied range tiles from the considered set of target tiles
					for (int i = rangeTiles.Num() - 1; i > -1; i--)
					{
						if (rangeTiles[i]->GetOccupied())
						{
							rangeTiles.RemoveAt(i);
							//UE_LOG(LogTemp, Warning, TEXT("Removed range tile at i %d"), i);
						}
					}

					if (rangeTiles.Num() > 0)
						targetTile = rangeTiles[0];

					//TODO
					//What if 5 enemies and 1 player remain
					// What if all the range tiles are occupied

					if (!targetTile)
					{
						//UE_LOG(LogTemp, Warning, TEXT("No range tiles to pick from smh"));
						targetTile = targetPlayer->GetMyTile();
					}
				}
				else //We're not going to move so return myTile_ occupied bool to true
				{
					if (myTile_)
						myTile_->SetOccupied(true);
					if (aiManager)
						aiManager->FinishedMoving();
				}
			}
			else
			{
				//We don't have a player target, find the closest one.
				FindTheClosestPlayer();
				return;
			}
			if (bGoingToMove)
			{
				MoveToTheTileWithinRangeOfThisTile(myTile_, targetTile);
			}
			gridManager->ClearHighlighted(); //Need to clear highlighted here so that different enemies don't get the highlighted tiles of each other
		}

		bWillMoveAgain = false; //Pickmovment destination is always the last one to be called in terms of movement
	}
	else //Only calculate the path if we have our current tile, otherwise, find your tile
	{
		bCannotFindTile = true;
	}
}
void AEnemyBaseGridCharacter::StartEnemyTurn()
{
	bWillMoveAgain = true; //Ready to obtain a new item
	targetItem = nullptr;
	CheckIfWeHaveAnyTargetItems();

}

void AEnemyBaseGridCharacter::Selected()
{
	UpdateOriginTile();
	if (gridManager && pathComp && originTile)
	{
		//Show the enemy's movement and attack ranges
		gridManager->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(), TILE_ENMH,0);
		gridManager->UpdateCurrentTile(originTile, statsComp->GetStatValue(STAT_WRS), statsComp->GetStatValue(STAT_WDS), TILE_ATK, statsComp->GetStatValue(STAT_PURE));
	}
}

void AEnemyBaseGridCharacter::NotSelected()
{
	if (gridManager)
		gridManager->ClearHighlighted();
}

void AEnemyBaseGridCharacter::ExecuteChosenAttack()
{
	if (gridManager)
	{
		ATile* myTile_ = GetMyTile();
		if (myTile_)
		{
			gridManager->ClearHighlighted();
			gridManager->UpdateCurrentTile(myTile_, statsComp->GetStatValue(STAT_WRS), statsComp->GetStatValue(STAT_WDS), TILE_ENM, statsComp->GetStatValue(STAT_PURE));
			if (targetPlayer)
			{
				if (targetPlayer->GetMyTile()->GetHighlighted() == TILE_ENM) //Is the player within attack range
				{
					if (btlCtrl)
						btlCtrl->FocusOnGridCharacter(this, 0.25f);
					AttackUsingWeapon(targetPlayer);
				}
				else
				{
					if (aiManager)
						aiManager->FinishedAttacking();

					if (crdItems.Num() > 0) //Look for new items on your next turn as someone may take your item on their turn
						crdItems.Empty();
				}
			}
			else //If you don't have a target yet, finish the attacking phase
			{
				if (aiManager)
					aiManager->FinishedAttacking();
				if (crdItems.Num() > 0)
					crdItems.Empty();
			}
		}
		else //Could not find the tile, don't break, finish the turn instead
		{
			if (aiManager)
				aiManager->FinishedAttacking();
			if (crdItems.Num() > 0)
				crdItems.Empty();
		}
	}
}

void AEnemyBaseGridCharacter::ActivateWeaponAttack()
{
	if (actionTargets[0])
		actionTargets[0]->GridCharTakeDamage(1.0f, this);
	if (aiManager)
		aiManager->FinishedAttacking();

	if (statsComp->AddTempCRD(CRD_ATK))
	{
		if (crdManager)
			crdManager->UpdateFavor(false);
	}
}

void AEnemyBaseGridCharacter::MoveAccordingToPath()
{
	if (movementPath.Num() > 0)
	{
		FVector direction = movementPath[movementPath.Num() - 1] - GetActorLocation();
		if (FMath::Abs(movementPath[movementPath.Num() - 1].X - GetActorLocation().X) > 10.0f || FMath::Abs(movementPath[movementPath.Num() - 1].Y - GetActorLocation().Y) > 10.0f)
		{
			AddMovementInput(direction, 1.0f);
		}
		else
		{
			movementPath.RemoveAt(movementPath.Num() - 1);
			if (movementPath.Num() == 0)
			{
				bMoving = false;
				if (!bWillMoveAgain) //If we have obtained an item, we're still gonna call pick movement destination so don't finish moving just yet
					// Will be false, once pickMovementDestination is called. If we have an  item pickMovementDestination will be called after colliding with the item
				{
					if (aiManager)
					{
						aiManager->FinishedMoving();
					}
				}
			}
		}
	}
}

void AEnemyBaseGridCharacter::FindTheClosestPlayer()
{
	//Not using dijkstra as precision is not important. We're assuming that the displacement is the shortest distance without checking the actual path through the tiles
	if (btlManager)
	{
		TArray<APlayerGridCharacter*> pchars = btlManager->GetDeployedPlayers();
		FVector myLoc = GetActorLocation();

		if (pchars.Num() > 0)
		{
			float min = 10000.0f;
			for (int i = 0; i < pchars.Num(); i++)
			{
				float distance = (myLoc - pchars[i]->GetActorLocation()).Size();
				if (distance < min)
				{
					targetPlayer = pchars[i];
					min = distance;
				}
			}
		}

		MoveCloserToTargetPlayer(nullptr); //Try again
	}
}

void AEnemyBaseGridCharacter::MoveToTheTileWithinRangeOfThisTile(ATile* startingTile_, ATile* targetTile_)
{
	if (targetTile_)
	{
		if (movementPath.Num() > 0)
			movementPath.Empty();

		if (!startingTile_) //No need to turn bCannotFindTile to true as startingTile_ will be a nullptr only when trying to get an item which always happens at the beginning of the turn which means the enemy is still standing.
			startingTile_ = GetMyTile();

		pathComp->SetCurrentTile(startingTile_);
		pathComp->SetTargetTile(targetTile_);
		movementPath = pathComp->GetPath(); //Get the whole path towards the target
		TArray<ATile*> tPath = pathComp->GetMovementPath();
		//Remove the non-highlighted tiles so you don't go to a tile that's out of reach
		if (!targetItem) //If we have a target item, that means we've already checked whether there's a path to that item. No need to check again
		{
			for (int i = 0; i < tPath.Num(); i++)
			{
				if (tPath[i]->GetHighlighted() != TILE_ENM)
				{
					tPath.RemoveAt(i);
					movementPath.RemoveAt(i);
				}
			}
		}
		//Make sure the last tile we can reach is not occupied. 
		//Set it to occupied as well before actually moving to avoid two enemies deciding to go for the same tile
		if(tPath.Num() > 0 && movementPath.Num()>0 && pathComp)
			pathComp->AdjustPath(tPath[0], movementPath);

		//Adjust path also sets the occupied flag on the final tile we can reach
		bMoving = true;

	}

}

void AEnemyBaseGridCharacter::CheckIfWeHaveAnyTargetItems()
{
	UpdateOriginTile();

	//Assuming we are going to move
	if (originTile)
		originTile->SetOccupied(false);

	if (crdItems.Num() > 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("crdItems > 0"));
		if (gridManager)
		{
			gridManager->ClearHighlighted();
			gridManager->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(), TILE_ENM,0);

			for (int i = 0; i < crdItems.Num(); i++)
			{
				if (crdItems[i])
				{
					//TODO
					//Check if we need the item first before marking it
					

					//If we have detected an item, make sure it's within movement range before marking it
					ATile* tile_ = crdItems[i]->GetMyTile();
					if (tile_)
					{
						//UE_LOG(LogTemp, Warning, TEXT("Got item's tile %d"), tile_->GetHighlighted());
						if (tile_->GetHighlighted() == TILE_ENM)
						{
						//	UE_LOG(LogTemp, Warning, TEXT("Item's tile is reachable"));
							if (crdItems[i]->MarkItem(this)) //True, means we've marked this items as our target and no other enemy should go for it
							{
							//	UE_LOG(LogTemp, Warning, TEXT("Item has been marked"));
								targetItem = crdItems[i];
								break;
							}
						}
					}

				}
			}
		}
	}		

	if (targetItem) //If we have a target item, then go there
	{
		//UE_LOG(LogTemp, Warning, TEXT("Move to get item"));
		MoveToTheTileWithinRangeOfThisTile(nullptr, targetItem->GetMyTile());
	}
	else //Otherwise, just go to your target player
	{
		//UE_LOG(LogTemp, Warning, TEXT("Couldn't find item so now going after player"));
		MoveCloserToTargetPlayer(nullptr);
	}
}

void AEnemyBaseGridCharacter::ItemIsUnreachable(ATile* startingTile_)
{
	targetItem = nullptr;

	if (movementPath.Num() > 0)
		movementPath.Empty();

	MoveCloserToTargetPlayer(startingTile_);
}

void AEnemyBaseGridCharacter::DetectItem(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_,
	bool bFromSweep_,
	const FHitResult &sweepResult_)
{
	if (otherActor_ != nullptr && otherActor_ != this && otherComp_ != nullptr)
	{
		ACrowdItem* item_ = Cast<ACrowdItem>(otherActor_);
		if (item_)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Found item"));
			crdItems.Push(item_);
		}

	}
}

void AEnemyBaseGridCharacter::TakeItem(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_,
	bool bFromSweep_,
	const FHitResult &sweepResult_)
{
	if (otherActor_ != nullptr && otherActor_ != this && otherComp_ != nullptr)
	{
		ACrowdItem* item_ = Cast<ACrowdItem>(otherActor_);

		if (item_) //If we've reached the item we planned to reach then obtain it and go to the closest player
		{
			//TODO
			//Get the item's value and update stats

			item_->Obtained(GetActorLocation());

			if (item_ != targetItem)//We've obtained an item that we did not mark, we need to let it tell the enemy that marked that it's gone
				item_->ItemWasObtainedByAnEnemyThatDidNotMarkIt();

			ItemIsUnreachable(item_->GetMyTile()); //You got the item, reset and go to the player starting from the item_ tile

			item_->Destroy();
		}
	}
}

void AEnemyBaseGridCharacter::ScaleLevelBaseOnArchetype(int level_, int archetype_)
{
	//TODO
}

///Summary of movement
/*
- The enemy's detectItems collider collides with items and those are pushed into the crdItems array.
- On the beginning of the enemy's turn, the enemy checks whether any of the items inside the array are reachable and can be marked (i.e. haven't been marked by a different enemy yet)
- If an enemy finds such an item, it'll go to it, once the collision occurs with the item, ItemIsUnreachable is called which calls MoveCloserToTargetPlayer and passes in the item's tile as the starting tile.
- The enemy starts moving from the item's tile to a tile close to the player and within the enemy's attack range.
- If the enemy does not find a reachable item, then they go straight to the player from their current tile.

*/
