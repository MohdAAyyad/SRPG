// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle.h"
#include "DestructibleComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Tile.h"

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

}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();

}

void AObstacle::AddObstructedTile(ATile* tile_) //Called the tiles the obstacle obstructs
{
	if (!obstructedTiles.Contains(tile_))
	{
		obstructedTiles.Push(tile_);
	}
}

void AObstacle::ObstacleTakeDamage(float damage_)
{
	hp -= damage_;
	if (hp <= 0.5f)
	{
		FTimerHandle timeToDestroyHandle;
		mesh->ApplyDamage(2.0f, GetActorLocation(), FVector::ZeroVector, 1.0f);
		mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetWorld()->GetTimerManager().SetTimer(timeToDestroyHandle, this, &AObstacle::DelayedDestroy, 3.0f, false);
	}
}

void AObstacle::DelayedDestroy()
{
	//Tell the tiles you obstructed that they can be traveresed now then die
	for (int i = 0; i < obstructedTiles.Num(); i++)
	{
		obstructedTiles[i]->SetTraversable(true);
	}
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
