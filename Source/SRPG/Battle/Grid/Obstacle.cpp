// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle.h"
#include "DestructibleComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Tile.h"
#include "ObstaclesManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PostProcessComponent.h"

// Sets default values
AObstacle::AObstacle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;
	root->SetMobility(EComponentMobility::Static);

	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	staticMesh->SetupAttachment(root);
	staticMesh->SetRenderCustomDepth(true);
	staticMesh->SetCustomDepthStencilValue(0);

	box = CreateDefaultSubobject<UBoxComponent>(TEXT("bOX"));
	box->SetupAttachment(root);
	box->SetCollisionProfileName("DestructibleBox");



	bCanBeRemoved = true;
	bCanBeTargeted = false;

	ppComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	ppComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();


	//Delay adding to obstacle maanger to ensure the tiles have been added to the obstacle's tile array in case the obstacle manager decides to destroy the obstacle
	//This is to avoid a race condition where the obstacle is destroyed before the tiles are added to the array potentially causing the tiles to remain non-traversable
	FTimerHandle timeToAddHandle;
	GetWorld()->GetTimerManager().SetTimer(timeToAddHandle, this, &AObstacle::AddObstacleToObstacleManager, 2.0f, false);

}

void AObstacle::AddObstacleToObstacleManager()
{
	if (obstacleManager)
	{
		if(bCanBeTargeted) //Only add obstacles that can be targeted
			obstacleManager->AddObstacle(this, bCanBeRemoved);
	}
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
//overriden in children
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

bool AObstacle::GetCanBeTargted()
{
	return bCanBeTargeted;
}


void AObstacle::ActivateOutline(bool value_)
{
	if (bCanBeTargeted)
	{
		if (value_)
			staticMesh->SetCustomDepthStencilValue(6);
		else
			staticMesh->SetCustomDepthStencilValue(0);
	}
}

void AObstacle::TargetedOutline()
{
	if (bCanBeTargeted)
	{
		staticMesh->SetCustomDepthStencilValue(2);
	}
}