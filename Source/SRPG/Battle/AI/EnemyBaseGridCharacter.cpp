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
	attackRange = 0;
	bWillMoveAgain = true;

}

void AEnemyBaseGridCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (detectionRadius)
		detectionRadius->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBaseGridCharacter::DetectItem);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBaseGridCharacter::TakeItem);

	btlCtrl = Cast<ABattleController>(GetWorld()->GetFirstPlayerController());
}
void AEnemyBaseGridCharacter::SetManagers(AAIManager* ref_, AGridManager* gref_, ABattleManager* bref_)
{
	//Called by AI Manager on Beginplay
	aiManager = ref_;
	gridManager = gref_;
	btlManager = bref_;
}

void AEnemyBaseGridCharacter::PickMovementDestination()
{

	//Assuming we are going to move
	if (originTile)
		originTile->SetOccupied(false);

	bool bGoingToMove = false;

	//Highlight your movement tiles
	if (gridManager && pathComp)
	{
		gridManager->ClearHighlighted();
		gridManager->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(), TILE_ENM); //4 Reachable by enemy
		//Checks if we have a player reference first, otherwise, move to target tile
		if (targetPlayer)
		{
			TArray<ATile*> rangeTiles;
			//Get the tiles that put the enemy within range of the player.
			rangeTiles = gridManager->GetTilesWithinAttackRange(attackRange, targetPlayer->GetMyTile());
			if (!rangeTiles.Contains(GetMyTile())) //Check if we're not within attacking range
			{
				targetTile = nullptr;
				bGoingToMove = true;

				//Remove the occupied range tiles from the considered set of target tiles
				for (int i = rangeTiles.Num()-1 ; i > -1 ; i--)
				{
					if (rangeTiles[i]->GetOccupied())
					{
						rangeTiles.RemoveAt(i);
						//UE_LOG(LogTemp, Warning, TEXT("Removed range tile at i %d"), i);
					}
				}

				if(rangeTiles.Num()>0)
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
			else //We're not going to move so return the originTile occupied bool to true
			{
				ATile* tile_ = GetMyTile();
				if (tile_)
					tile_->SetOccupied(true);
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
			MoveToTheTileWithinRangeOfThisTile(targetTile);
		}
		gridManager->ClearHighlighted(); //Need to clear highlighted here so that different enemies don't get the highlighted tiles of each other
	}

	bWillMoveAgain = false; //Pickmovment destination is always the last one to be called in terms of movement
}


void AEnemyBaseGridCharacter::DetectItem(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_,
	bool bFromSweep_,
	const FHitResult &sweepResult_)
{
	if (btlManager)
	{
		if (btlManager->GetPhase() == BTL_ENM)
		{
			if (otherActor_ != nullptr && otherActor_ != this && otherComp_ != nullptr)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Found Another actor"));
				/*if (!targetPlayer)
				{
					//TODO compare the new target with the original one before commiting to it.
					targetPlayer = Cast< APlayerGridCharacter>(otherActor_);
					if (targetPlayer)
						UE_LOG(LogTemp, Warning, TEXT("Found player"));
				}
				*/
				//TODO
				//Check if you need that item 
				ACrowdItem* item_ = Cast<ACrowdItem>(otherActor_);
				if (item_)
				{
					UE_LOG(LogTemp, Warning, TEXT("Found item"));
					crdItems.Push(item_);
				}

			}
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
	if (btlManager)
	{
		if (btlManager->GetPhase() == BTL_ENM)
		{
			if (otherActor_ != nullptr && otherActor_ != this && otherComp_ != nullptr)
			{
				ACrowdItem* item_ = Cast<ACrowdItem>(otherActor_);

				if (item_) //If we've reached the item we planned to reach then obtain it and go to the closes player
				{
					//TODO
					//Get the item's value

					bWillMoveAgain = true;
					item_->Obtained(GetActorLocation());

					PickMovementDestination();

					if (item_ != targetItem)//We've obtained an item that we did not mark, we need to let it tell the enemy that marked that it's gone
						item_->ItemWasObtainedByAnEnemyThatDidNotMarkIt();
				}
			}
		}
	}
}

void AEnemyBaseGridCharacter::EnableDetectionCollision()
{
	bWillMoveAgain = true; //Ready to obtain a new item
	targetItem = nullptr;
	//The reason CheckIfWeHaveAnyTargetItems function call is delayed is to avoid a race condition between it and DetectItem
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &AEnemyBaseGridCharacter::CheckIfWeHaveAnyTargetItems, 0.3f, false);

}

void AEnemyBaseGridCharacter::Selected()
{
	UpdateOriginTile();
	if (gridManager && pathComp && originTile)
	{
		//Show the enemy's movement and attack ranges
		gridManager->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(), TILE_ENMH);
		gridManager->UpdateCurrentTile(originTile, attackRowSpeed, attackDepth, TILE_ATK);
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
		gridManager->UpdateCurrentTile(originTile, attackRowSpeed, attackDepth,4);
		if (targetPlayer)
		{
			if (targetPlayer->GetMyTile()->GetHighlighted() == TILE_ENM) //Is the player within attack range
			{
				if (btlCtrl)
					btlCtrl->FocusOnGridCharacter(this,0.25f);
				AttackUsingWeapon(targetPlayer);
				gridManager->ClearHighlighted(); //Need to clear highlighted to make sure enemies don't attack the player when they're out of range when multiple enemies are targeting the same player
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
			if (crdItems.Num() > 0) //Look 
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

	//Placeholder must be done in stat component
	if (currentCrdPoints >= 100)
	{
		currentCrdPoints -= 100;
		crd += 1;
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
				UpdateOriginTile();
				bMoving = false;
				if (!bWillMoveAgain) //If we have obtained an item, we're still gonna call pick movement destination so don't finish moving just yet
					// Will be false, once pick movement destination is called
				{
					if (aiManager)
						aiManager->FinishedMoving();
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

		PickMovementDestination(); //Try again
	}
}

void AEnemyBaseGridCharacter::MoveToTheTileWithinRangeOfThisTile(ATile* tile_)
{
	if (tile_)
	{
		if (movementPath.Num() > 0)
			movementPath.Empty();

		pathComp->SetCurrentTile(originTile);
		pathComp->SetTargetTile(tile_);
		movementPath = pathComp->GetPath(); //Get the whole path towards the target
		TArray<ATile*> tPath = pathComp->GetMovementPath();
		//Remove the non-highlighted tiles so you don't go to a tile that's out of reach
		for (int i = 0; i < tPath.Num(); i++)
		{
			if (tPath[i]->GetHighlighted() != TILE_ENM)
			{
				tPath.RemoveAt(i);
				movementPath.RemoveAt(i);
			}
		}
		//Make sure the last tile we can reach is not occupied. 
		//Set it to occupied as well before actually moving to avoid two enemies deciding to go for the same tile
		if(tPath.Num() > 0 && pathComp)
			pathComp->AdjustPath(tPath[0], movementPath);

		bMoving = true;

	}

}

void AEnemyBaseGridCharacter::CheckIfWeHaveAnyTargetItems()
{
	UpdateOriginTile();
	if (crdItems.Num() > 0)
	{
		if (gridManager)
		{
			gridManager->ClearHighlighted();
			gridManager->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(), TILE_ENM);

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
						if (tile_->GetHighlighted() == TILE_ENM)
						{
							if (crdItems[i]->MarkItem(this)) //True, means we've marked this items as our target and no other enemy should go for it
							{
								targetItem = crdItems[i];
								break;
							}
						}
					}

				}
			}
		}
		crdItems.Empty();
	}		

	if (targetItem) //If we have a target item, then go there
	{
		MoveToTheTileWithinRangeOfThisTile(targetItem->GetMyTile());
	}
	else //Otherwise, just go to your target player
	{
		PickMovementDestination();
	}
}

void AEnemyBaseGridCharacter::ItemIsUnreachable()
{
	targetItem = nullptr;
	if (bMoving)
		bMoving = false;
	if (movementPath.Num() > 0)
		movementPath.Empty();
	PickMovementDestination();
}