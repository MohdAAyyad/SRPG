// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle.h"
#include "DestructibleComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Tile.h"
#include "ObstaclesManager.h"

// Sets default values
AObstacle::AObstacle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	mesh = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(root);

	box = CreateDefaultSubobject<UBoxComponent>(TEXT("bOX"));
	box->SetupAttachment(root);
	box->SetCollisionProfileName("DestructibleBox");

	hp = 30.0f;

	bCanBeRemoved = true;

}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();


	//Delay adding to obstacle maanger to ensure the tiles have been added to the obstacle's tile array in case the obstacle manager decides to destroy the obstacle
	//This is to avoid a race condition where the obstacle is destroyed before the tiles are added to the array potentially causing the tiles to remain non-traversable
	FTimerHandle timeToAddHandle;
	GetWorld()->GetTimerManager().SetTimer(timeToAddHandle, this, &AObstacle::AddObstacleToObstacleManager, 1.0f, false);

}

void AObstacle::AddObstacleToObstacleManager()
{
	if (obstacleManager)
		obstacleManager->AddObstacle(this, bCanBeRemoved);
}

bool AObstacle::AddObstructedTile(ATile* tile_) //Called the tiles the obstacle obstructs
{
	if (!obstructedTiles.Contains(tile_))
	{
		obstructedTiles.Push(tile_);
	}
	return true;
}

void AObstacle::ObstacleTakeDamage(float damage_, int statusEffect_)
{
	hp -= damage_;
	if (hp <= 0.5f)
	{
		FTimerHandle timeToDestroyHandle;
		mesh->ApplyDamage(5.0f, GetActorLocation(), FVector::ZeroVector, 1.0f);
		mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetWorld()->GetTimerManager().SetTimer(timeToDestroyHandle, this, &AObstacle::DelayedDestroy, 2.0f, false);
	}
}

void AObstacle::DelayedDestroy()
{
	//Tell the tiles you obstructed that they can be traveresed now then die
	for (int i = 0; i < obstructedTiles.Num(); i++)
	{
		obstructedTiles[i]->SetTraversable(true);
	}
	//Tell the obstacle manager that you're done
	if (obstacleManager)
		obstacleManager->RemoveObstacle(this);
	else
		Destroy();
}

bool AObstacle::IsAnyOfMyTilesHighlighted(int highlightIndex_)
{
	//Called by the battle controller to make sure the player is attacking an obstacled within range
	for (int i = 0; i < obstructedTiles.Num(); i++)
	{
		if (obstructedTiles[i]->GetHighlighted() == highlightIndex_)
			return true;
	}

	return false;
}

TArray<ATile*> AObstacle::GetObstructedTiles()
{
	return obstructedTiles;
}
