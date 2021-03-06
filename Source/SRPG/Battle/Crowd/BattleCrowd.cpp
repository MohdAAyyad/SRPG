// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleCrowd.h"
#include "../BattleManager.h"
#include "../GridCharacter.h"
#include "../AI/AIManager.h"
#include "../Grid/GridManager.h"
#include "../Grid/Tile.h"
#include "../Grid/ObstaclesManager.h"
#include "../BattlePawn.h"
#include "Definitions.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "CrowdItem.h"
#include "../../Audio/AudioMnager.h"
#include "../Player/PlayerGridCharacter.h"


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
	bReplicates = true;

	maxNumOfObstaclesToDestroy = 2;
	currentNumOfObstaclesDestroyed = 0;

	numOfTurnsToAnDestroyObstacle = 3;
	numOfTurnsPassedSinceLastObstacleWasDestroyed = 0;

	chosenObstacle = nullptr;
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
	numOfTurnsPassedSinceLastObstacleWasDestroyed++; //Add 1 to it every turn

	//If favor is higher than 70  or less than 30, and we don't have a champion or a villain then elect a champion and/or a villain
	if (playerFavor >= 0.7f || playerFavor <= 0.3f && !bPermaChampion)
	{
		if(!champion)
			ElectChampion();
		if (!villain)
			ElectVillain();
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
			//Elect a new champion and a new villain
			ElectChampion(); 
			ElectVillain();
			playerFavor = 0.5f;
			turnsSinceChampionAndVillainWereElected = 0;
		}
	}
	//If favor is less than 70 and higher than 30, and we have a champion or a villain but not a perma champion then unelect both and reset the turns counter
	else if (((playerFavor < 0.7f && playerFavor > 0.3f) && (champion || villain) && !bPermaChampion))
	{
		//Not resetting the c and v ptrs to nullptr so that the next time someone else gets elected
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

	DecideOnActionForCurrentPhase();
}

void ABattleCrowd::DecideOnActionForCurrentPhase()
{
	//Check for obstacles first

	if (numOfTurnsPassedSinceLastObstacleWasDestroyed >= numOfTurnsToAnDestroyObstacle && currentNumOfObstaclesDestroyed < maxNumOfObstaclesToDestroy)
	{
		numOfTurnsPassedSinceLastObstacleWasDestroyed = 0; //Reset the count
		MoveBattlePawnOverObstacle();
	}
	else //Otherwise, just spawns items
	{
		SpawnItems();
	}

}

void ABattleCrowd::MoveBattlePawnOverObstacle()
{
	if (obstacleManager)
	{
		//Choose the obstacle
		chosenObstacle = obstacleManager->GetAnObstacleAtRandom();

		if (chosenObstacle)
		{
			//Tell the battle pawn to move over the chosenObstacle
			ABattlePawn* btlPawn = Cast<ABattlePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
			if (btlPawn)
			{
				btlPawn->LockOnActor(this, chosenObstacle);
			}
		}
		else
		{
			SpawnItems();
		}
	}
	else //Just being cautious
	{
		SpawnItems();
	}
}

void ABattleCrowd::DestroyObstacle()
{

	if (chosenObstacle)
	{
		FVector spawnLoc = chosenObstacle->GetActorLocation();
		spawnLoc.Z += 600.0f;
		//First check if the obstacle is an elemental hazard
		AElementalHazard* hazard = Cast<AElementalHazard>(chosenObstacle);
		if (hazard)
		{
			int projIndex = PickProjectileBasedOnHazardCurrentState(hazard->GetCurrentState());
			if (projIndex >= 0 && projIndex < crowdProjectiles.Num())
			{
				currentNumOfObstaclesDestroyed++;
				GetWorld()->SpawnActor<ACrowdProjectile>(crowdProjectiles[projIndex], spawnLoc, FRotator::ZeroRotator);
			}
			else
			{
				SpawnItems(); //We couldn't find a suitable action, so just spawn items
			}
		}
		else //If it's not a hazard, spawn the first projectile above the obstacle
		{
			currentNumOfObstaclesDestroyed++;
			GetWorld()->SpawnActor<ACrowdProjectile>(crowdProjectiles[0], spawnLoc, FRotator::ZeroRotator);
		}

		chosenObstacle = nullptr;
	}
	FTimerHandle endHandle;
	GetWorld()->GetTimerManager().SetTimer(endHandle, this, &ABattleCrowd::EndPhase, 4.0f, false);


}

void ABattleCrowd::BattlePawnHasFinishedMoving()
{
	FTimerHandle actionHandle;
	GetWorld()->GetTimerManager().SetTimer(actionHandle, this, &ABattleCrowd::DestroyObstacle, 1.0f, false);
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
			if (!highlightedTiles_[tileChoice]->GetOccupied() && highlightedTiles_[tileChoice]->GetTraversable())
			{
				FVector loc = highlightedTiles_[tileChoice]->GetActorLocation();
				loc.Z += 50.0f;
				if (crowdItems[itemChoice])
				{
					ACrowdItem* item_ =GetWorld()->SpawnActor<ACrowdItem>(crowdItems[itemChoice], loc, FRotator::ZeroRotator);
					if (item_)
					{
						spawnedItems.Push(item_);
						item_->SetBtlAndCrdManagers(btlManager, this);
					}
				}
			}
		}
	}

	EndPhase();
}

void ABattleCrowd::SpawnItemsAtLoc(FVector loc_)
{
	ACrowdItem* item_ = GetWorld()->SpawnActor<ACrowdItem>(crowdItems[1], loc_, FRotator::ZeroRotator);
	spawnedItems.Push(item_);
	item_->SetBtlAndCrdManagers(btlManager, this);
}
void ABattleCrowd::ElectChampion()
{
	if (btlManager && aiManager)
	{

		if (playerFavor >= 0.7f) //Elect a champ from the player camp
		{
			champion = btlManager->GetPlayerWithHighestStat(STAT_CRD,champion);
			if (audioMgr)
				audioMgr->SwitchMusic(2);
		}
		else //Otherwise, elect an enemy to be the champ
		{
			champion = aiManager->GetEnemyWithHighestStat(STAT_CRD, champion);
			if (audioMgr)
				audioMgr->SwitchMusic(3);
		}

		//Call the needed functions on the champ
		if(champion)
			champion->SetChampionOrVillain(true);
	}
}

void ABattleCrowd::ElectVillain()
{
	if (btlManager && aiManager)
	{
		if (playerFavor <= 0.3f) //Elect a villain from the player camp
		{
			villain = btlManager->GetPlayerWithLowestStat(STAT_CRD, villain);
		}
		else //Otherwise, elect an enemy to be the villain
		{
			villain = aiManager->GetEnemyWithLowestStat(STAT_CRD, villain);
		}

		//Call the needed functions on the villain
		if(villain)
			villain->SetChampionOrVillain(false);
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

	playerFavor = int(playerFavor * 100) * 0.01f;
}
void ABattleCrowd::UpdateFavor(bool bPlayerOrEnemy_) //Called when the player or the enemy gain a CRD point
{
	if (bPlayerOrEnemy_) //True player, false enemy
	{
		playerFavor += favorIncrement;
		if (playerFavor >= 1.0f)
			playerFavor = 1.0f;
	}
	else
	{
		playerFavor -= favorIncrement;
		if (playerFavor <= 0.0f)
			playerFavor = 0.0f;
	}

	//playerFavor = int(playerFavor * 100) * 0.01;
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

void ABattleCrowd::SetPermaChampion(bool bvalue_)
{
	bPermaChampion = bvalue_;
	if (!bPermaChampion)
		champion = nullptr;
}

void ABattleCrowd::ChampVillainIsDead(bool bChamp_)
{
	if (bChamp_)
	{
		champion = nullptr;
		if (villain)
		{
			villain->UnElect();
			villain = nullptr;
		}
	}
	else
	{
		villain = nullptr;
	}
}

void ABattleCrowd::IAmTheNewChampion(AGridCharacter* gchar_)
{
	if (champion)
		champion->UnElect();
	champion = gchar_;
}

void ABattleCrowd::FlipFavorMeter()
{
	if (playerFavor >= 0.7f)//Meter has flipped from player to enemy so switch music to villain theme
	{
		if (audioMgr)
			audioMgr->SwitchMusic(3);
	}
	else
	{
		if (audioMgr)
			audioMgr->SwitchMusic(2);
	}
	playerFavor = 1 - playerFavor;
}

void ABattleCrowd::FlipFavorMeter(AGridCharacter* gchar_)
{
	//Check if the favor is 70:30 in favor of gchar_. If it is, we're all good, if not, flip it
	//Check whether the champ is a player or an enemy
	APlayerGridCharacter* pChar_ = Cast<APlayerGridCharacter>(gchar_);

	if (pChar_)
	{
		if (playerFavor <= 0.3f) //If the meter is not in favor of the player, flip it
		{
			FlipFavorMeter();
		}
		else //If it's higher than 0.3f, then make the player's favor into 0.7f
		{
			playerFavor = 0.7f;
			if (audioMgr)
				audioMgr->SwitchMusic(2);
		}
	}
	else
	{
		if (playerFavor >= 0.7f) //If the meter is not in favor of the enemy, flip it
		{
			FlipFavorMeter();
		}
		else //If it's lower than 0.7f, then make the enemy's favor into 0.7f
		{
			playerFavor = 0.3f;
			if (audioMgr)
				audioMgr->SwitchMusic(3);
		}
	}
}

void ABattleCrowd::UpdateFavorForChamp(AGridCharacter* gchar_, int times_)
{
	//Check whether the champ is a player or an enemy
	APlayerGridCharacter* pChar_ = Cast<APlayerGridCharacter>(gchar_);

	if (pChar_)
	{
		playerFavor += favorIncrement * times_;
		if (playerFavor > 1.0f)
			playerFavor = 1.0f;
	}
	else
	{
		playerFavor -= favorIncrement * times_;
		if (playerFavor < 0.0f)
			playerFavor = 0.0f;
	}
}

void ABattleCrowd::UnElect(bool champ_)
{
	if (champ_)
	{
		if (champion)
		{
			champion->UnElect();
			champion = nullptr;
		}
	}
	else
	{
		if (villain)
		{
			villain->UnElect();
			villain = nullptr;
		}
	}
}

bool ABattleCrowd::GetPermaStatus()
{
	return bPermaChampion;
}


void ABattleCrowd::EndPhase()
{
	if (btlManager)
		btlManager->NextPhase();
}

int ABattleCrowd::PickProjectileBasedOnHazardCurrentState(CurrentElemntalStat state_)
{
	int chance = 0;
	switch (state_)
	{
	case CurrentElemntalStat::NONE:
	case CurrentElemntalStat::PARALYSIS:
	case CurrentElemntalStat::EXPLOSION:
	case CurrentElemntalStat::STEAM:
		return -1;

	case CurrentElemntalStat::OIL:
		return EFFECT_BURN; //Can only use fire on oil
	case CurrentElemntalStat::WATER:
		//Water can either be "burned" "frozen" or "electrified"
		chance = FMath::RandRange(0, 99);

		if (chance >= 0 && chance < 33)
			return EFFECT_BURN;
		else if (chance >= 33 && chance < 66)
			return EFFECT_FREEZE - 1;//FREEZE is 5, the array's max size is 4. There was a mix up here cause bleeding is 4, and it should've been 5 but I don't want to go change the UI to accomodate that so 4 it is
		else
			return EFFECT_PARALYSIS;

	case CurrentElemntalStat::POISON:
		return EFFECT_BURN;

	case CurrentElemntalStat::FIRE:

		chance = FMath::RandRange(0, 100);

		if (chance >= 0 && chance < 50)
			return EFFECT_FREEZE - 1;
		else
			return EFFECT_POISON;

	case CurrentElemntalStat::FROZEN:
		return EFFECT_BURN;
	default:
		return -1;
	}
}