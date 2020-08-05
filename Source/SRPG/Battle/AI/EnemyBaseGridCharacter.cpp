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


AEnemyBaseGridCharacter::AEnemyBaseGridCharacter() :AGridCharacter()
{
	detectionRadius = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionRadius"));
	detectionRadius->SetupAttachment(RootComponent);
	detectionRadius->SetGenerateOverlapEvents(true);
	detectionRadius->SetCollisionProfileName("EnemyDetectionRadius");
	detectionRadius->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	targetPlayer = nullptr;
	targetTile = nullptr;
	attackRange = 0;

}

void AEnemyBaseGridCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (detectionRadius)
		detectionRadius->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBaseGridCharacter::DetectPlayer);			

	btlCtrl = Cast<ABattleController>(GetWorld()->GetFirstPlayerController());
}
void AEnemyBaseGridCharacter::SetManagers(AAIManager* ref_, AGridManager* gref_)
{
	//Called by AI Manager on Beginplay
	aiManager = ref_;
	gridManager = gref_;
}

void AEnemyBaseGridCharacter::PickMovementDestination()
{
	//Update your origin tile
	UpdateOriginTile();

	if (movementPath.Num() > 0)
		movementPath.Empty();

	//Assuming we are going to move
	if (originTile)
		originTile->SetOccupied(false);

	bool bGoingToMove = false;

	//Highlight your movement tiles
	if (gridManager && pathComp)
	{
		gridManager->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(), 4); //4 Reachable by enemy
		//Checks if we have a player reference first, otherwise, move to target tile
		if (targetPlayer)
		{
			TArray<ATile*> rangeTiles;
			//Get the tiles that put the enemy within range of the player.
			rangeTiles = gridManager->GetTilesWithinAttackRange(attackRange, targetPlayer->GetMyTile());
			if (!rangeTiles.Contains(originTile)) //Check if we're not within attacking range
			{
				targetTile = nullptr;
				bGoingToMove = true;

				//Remove the occupied range tiles from the considered set of target tiles
				for (int i = rangeTiles.Num()-1 ; i > -1 ; i--)
				{
					if (rangeTiles[i]->GetOccupied())
					{
						rangeTiles.RemoveAt(i);
						UE_LOG(LogTemp, Warning, TEXT("Removed range tile at i %d"), i);
					}
				}

				if(rangeTiles.Num()>0)
					targetTile = rangeTiles[0];

				//TODO
				//What if 5 enemies and 1 player remain
				// What if all the range tiles are occupied

				if (!targetTile)
				{
					UE_LOG(LogTemp, Warning, TEXT("No range tiles to pick from smh"));
					targetTile = targetPlayer->GetMyTile();
				}
			}
			else //We're not going to move so return the originTile occupied bool to true
			{
				if (originTile)
					originTile->SetOccupied(true);
				if (aiManager)
					aiManager->FinishedMoving();
			}
		}
		else
		{
			if (gridManager) //We haven't detected the player yet, so move to the arbitrary predetermined destination
			{
				bGoingToMove = true;
				targetTile = gridManager->GetTileFromRowAndOffset(rowIndexOfDeploymentTargetTile, offsetOfDeploymentTargetTile);
			}
		}
		if (bGoingToMove)
		{
			pathComp->SetCurrentTile(originTile);
			pathComp->SetTargetTile(targetTile);
			movementPath = pathComp->GetPath(); //Get the whole path towards the target
			TArray<ATile*> tPath = pathComp->GetMovementPath();
			//Remove the non-highlighted tiles so you don't go to a tile that's out of reach
			for (int i = 0; i < tPath.Num(); i++)
			{
				if (tPath[i]->GetHighlighted() != 4)
				{
					tPath.RemoveAt(i);
					movementPath.RemoveAt(i);
				}
			}
			//Make sure the last tile we can reach is not occupied. 
			//Set it to occupied as well before actually moving to avoid two enemies deciding to go for the same tile
			pathComp->AdjustPath(tPath[0], movementPath);
			 
		}
		gridManager->ClearHighlighted(); //Need to clear highlighted here so that different enemies don't get the highlighted tiles of each other
	}

	if (detectionRadius)
		detectionRadius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void AEnemyBaseGridCharacter::DetectPlayer(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_,
	bool bFromSweep_,
	const FHitResult &sweepResult_)
{
	if (otherActor_ != nullptr && otherActor_ != this && otherComp_ != nullptr)
	{
		if (!targetPlayer)
		{
			//TODO compare the new target with the original one before commiting to it.
			targetPlayer = Cast< APlayerGridCharacter>(otherActor_);
			if (targetPlayer)
				UE_LOG(LogTemp, Warning, TEXT("Found player"));
		}
	}
}

void AEnemyBaseGridCharacter::EnableDetectionCollision()
{
	if (detectionRadius)
		detectionRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//The reason PickMovementDestination function call is delayed is to avoid a race condition between it and DetectPlayer
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &AEnemyBaseGridCharacter::PickMovementDestination, GetWorld()->GetDeltaSeconds(), false);
}

void AEnemyBaseGridCharacter::Selected()
{
	UpdateOriginTile();
	if (gridManager && pathComp && originTile)
	{
		//Show the enemy's movement and attack ranges
		gridManager->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(), 5);
		gridManager->UpdateCurrentTile(originTile, attackRowSpeed, attackDepth, 1);
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
			if (targetPlayer->GetMyTile()->GetHighlighted() == 4) //Is the player within attack range
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
			}
		}
		else //If you don't have a target yet, finish the attacking phase
		{
			if (aiManager)
				aiManager->FinishedAttacking();
		}
	}
}

void AEnemyBaseGridCharacter::ActivateWeaponAttack()
{
	if (actionTargets[0])
		actionTargets[0]->GridCharTakeDamage(1.0f, this);
	if (aiManager)
		aiManager->FinishedAttacking();
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
				if (aiManager)
					aiManager->FinishedMoving();
			}
		}
	}
}