// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBaseGridCharacter.h"
#include "AIManager.h"
#include "../Player/PlayerGridCharacter.h"
#include "../Grid/Tile.h"
#include "../Grid/GridManager.h"
#include "Components/BoxComponent.h"


AEnemyBaseGridCharacter::AEnemyBaseGridCharacter() :AGridCharacter()
{
	detectionRadius = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionRadius"));
	detectionRadius->SetupAttachment(RootComponent);
	detectionRadius->SetGenerateOverlapEvents(true);
	detectionRadius->SetCollisionProfileName("EnemyDetectionRadius");

	targetPlayer = nullptr;
	targetTile = nullptr;
	attackRange = 0;

}
void AEnemyBaseGridCharacter::SetManagers(AAIManager* ref_, AGridManager* gref_)
{
	//Called by AI Manager on Beginplay
	aiManager = ref_;
	gridManager = gref_;
}

void AEnemyBaseGridCharacter::PickATargetTile()
{
	//Get the target tile. Called on begin play by AI manager
	if (gridManager)
	{
		targetTile = gridManager->GetTileFromRowAndOffset(rowIndexOfDeploymentTargetTile, offsetOfDeploymentTargetTile);
	}
}

void AEnemyBaseGridCharacter::MoveToTarget()
{
	//Checks if we have a player reference first, otherwise, move to target tile
	if (targetPlayer)
	{

	}
	else if (targetTile)
	{

	}
}