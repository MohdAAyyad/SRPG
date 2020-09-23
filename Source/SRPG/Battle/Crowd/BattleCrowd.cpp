// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCrowd.h"
#include "../BattleManager.h"
#include "../GridCharacter.h"
#include "../AI/AIManager.h"
#include "../Grid/GridManager.h"
#include "../Grid/Tile.h"
#include "Definitions.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "CrowdItem.h"

// Sets default values
ABattleCrowd::ABattleCrowd()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	playerFavor = 0.0f;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = root;
	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	widgetComp->SetupAttachment(root);
	widgetComp->SetVisibility(false);

	numOfItemsSpawnedPerTurn = 2;
	numOfTurnsToPassBeforeItemsSpawnedIsIncreased = 3;
	maxNumOfItemsSpawnedPerTurn = 6;
	favorIncrement = 0.06f;
	turnsSinceChampionAndVillainWereElected = 0;
	bPermaChampion = false;

	champion = nullptr;
	villain = nullptr;
}

// Called when the game starts or when spawned
void ABattleCrowd::BeginPlay()
{
	Super::BeginPlay();

	if (gridManager)
		neutralItemsTile = gridManager->GetTileFromRowAndOffset(rowIndexForNeutralItems, offsetForNeutralItems);
	
}

// Called every frame
void ABattleCrowd::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABattleCrowd::StartCrowdPhase()
{
	//If favor is higher than 70  or less than 30, and we don't have a champion or a villain then elect a champion and/or a villain
	if ((playerFavor >= 0.7f || playerFavor <= 0.3f) && (!champion || !villain))
	{
		ElectChampionAndVillain();
	}
	//If favor is higher than 70  or less than 30, and we have a champion or a villain but not a perma champion then start counting the turns since the c and v were first elected
	else if ((playerFavor >= 0.7f || playerFavor <= 0.3f) && (champion || villain) && !bPermaChampion)
	{
		turnsSinceChampionAndVillainWereElected++;
		if (turnsSinceChampionAndVillainWereElected >= 3) //If three turns have passed since the election, then unelect both and return the favor to a 50:50 ratio
		{
			if (champion)
			{
				champion->UnElect();
				champion = nullptr;
			}
			if (villain)
			{
				villain->UnElect();
				villain = nullptr;
			}
			playerFavor = 0.5f;
			turnsSinceChampionAndVillainWereElected = 0;
		}
	}
	//If favor is less than 70 and higher than 30, and we have a champion or a villain but not a perma champion then unelect both and reset the turns counter
	else if (((playerFavor < 0.7f && playerFavor > 0.3f) && (champion || villain) && !bPermaChampion))
	{
		if (champion)
		{
			champion->UnElect();
			champion = nullptr;
		}
		if (villain)
		{
			villain->UnElect();
			villain = nullptr;
		}
		turnsSinceChampionAndVillainWereElected = 0;
	}

	TArray<int> itemsToBeRemoved;
	//Check if spawned items should be destroyed
	if (spawnedItems.Num() > 0)
	{
		for (int i = 0; i < spawnedItems.Num(); i++)
		{
			if (spawnedItems[i])
			{
				if (spawnedItems[i]->CheckIfItemShouldBeDestroyed())
				{
					itemsToBeRemoved.Push(i);
				}
			}
		}
	}

	for (int i = 0; i < itemsToBeRemoved.Num(); i++)
	{
		spawnedItems.RemoveAt(itemsToBeRemoved[i]);
	}

	SpawnItems();
}

void ABattleCrowd::SpawnItems()
{
	TArray<ATile*> highlightedTiles_;
	//Check how many items to spawn
	if (btlManager)
	{
		if (numOfItemsSpawnedPerTurn + 1 < maxNumOfItemsSpawnedPerTurn) //Checking this first to avoid the mod operation if possible
		{
			if (btlManager->GetTotalNumberOfPhasesElapsed() % numOfTurnsToPassBeforeItemsSpawnedIsIncreased == 0)
			{
				numOfItemsSpawnedPerTurn++;
			}
		}
	}

	//Spawn items near the champion if we have one
	if (champion)
	{
		ATile* tile_ = champion->GetMyTile();
		if (tile_)
		{
			if (gridManager)
			{
				gridManager->ClearHighlighted();
				gridManager->UpdateCurrentTile(tile_, rowSpeedForSpawningItems, depthForSpawningItems, TILE_CRD,0);
			}
		}
	}
	else //Otherwise spawn them around the netural tile
	{
		if (neutralItemsTile)
		{
			if (gridManager)
			{
				gridManager->ClearHighlighted();
				gridManager->UpdateCurrentTile(neutralItemsTile, rowSpeedForSpawningItems, depthForSpawningItems, TILE_CRD,0);
			}
		}
	}

	//Get the highlighted tiles and spawn the items on them
	highlightedTiles_ = gridManager->GetHighlightedTiles();
	int itemChoice = 0;
	int tileChoice = 0;

	for (int i = 0; i < numOfItemsSpawnedPerTurn; i++)
	{
		itemChoice = FMath::RandRange(0, crowdItems.Num() - 1);
		tileChoice = FMath::RandRange(0, highlightedTiles_.Num() - 1);
		if (highlightedTiles_[tileChoice])
		{
			if (!highlightedTiles_[tileChoice]->GetOccupied())
			{
				FVector loc = highlightedTiles_[tileChoice]->GetActorLocation();
				loc.Z += 50.0f;
				if (crowdItems[itemChoice])
				{
					ACrowdItem* item_ =GetWorld()->SpawnActor<ACrowdItem>(crowdItems[itemChoice], loc, FRotator::ZeroRotator);
					spawnedItems.Push(item_);
					item_->SetBtlAndCrdManagers(btlManager, this);
				}
			}
		}
	}

	if (btlManager)
		btlManager->NextPhase();
}
void ABattleCrowd::ElectChampionAndVillain()
{
	if (!champion)
	{
		if (btlManager && aiManager)
		{
			AGridCharacter* champ = nullptr;

			if (playerFavor >= 0.7f) //Elect a champ from the player camp
			{
				champ = btlManager->GetPlayerWithHighestStat(STAT_CRD);
			}
			else //Otherwise, elect an enemy to be the champ
			{
				champ = aiManager->GetEnemyWithHighestStat(STAT_CRD);
			}

			//TODO
			//Call the needed functions on the champ
			champ->SetChampionOrVillain(true);
		}
	}
	
	if (!villain)
	{
		if (btlManager && aiManager)
		{
			AGridCharacter* villain_ = nullptr;

			if (playerFavor <= 0.3f) //Elect a villain from the player camp
			{
				villain_ = btlManager->GetPlayerWithLowestStat(STAT_CRD);
			}
			else //Otherwise, elect an enemy to be the villain
			{
				villain_ = aiManager->GetEnemyWithLowestStat(STAT_CRD);
			}

			//TODO
			//Call the needed functions on the villain
			villain_->SetChampionOrVillain(false);
		}
	}
}
void ABattleCrowd::CalculateFavorForTheFirstTime()
{
	float totalCRD = 0.0f; 
	float playerCRD = 0.0f; 
	float enemyCRD = 0.0f;

	if (aiManager && btlManager)
	{
		enemyCRD = aiManager->GetTotalStatFromDeployedEnemies(STAT_CRD);
		playerCRD = btlManager->GetTotalStatFromDeployedPlayers(STAT_CRD);
		totalCRD = playerCRD + enemyCRD;
	}

	playerFavor = playerCRD / totalCRD;
}
void ABattleCrowd::UpdateFavor(bool bPlayerOrEnemy_) //Called when the player or the enemy gain a CRD point
{
	if (bPlayerOrEnemy_) //True player, false enemy
	{
		playerFavor += favorIncrement;
	}
	else
	{
		playerFavor -= favorIncrement;
	}
}

void ABattleCrowd::RemoveSpawnedItem(ACrowdItem* item_)
{
	if (spawnedItems.Contains(item_))
	{
		spawnedItems.Remove(item_);
	}
}

void ABattleCrowd::AddCrowdWidgetToViewPort()
{
	if (widgetComp)
		widgetComp->GetUserWidgetObject()->AddToViewport();
}
