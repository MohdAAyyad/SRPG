// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Obstacle.h"

// Sets default values
ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = false;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(root);

	gridManager = nullptr;
	HighlightedIndex = -1;

	//Each tile will have 8 neighbors at max
	immediateNeighbors.Reserve(4);
	diagonalNeighbors.Reserve(4);
	gCost = hCost = fCost = 0;
	bTraversable = true;
	parentTile = nullptr;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	FHitResult hit;
	FVector end = GetActorLocation();
	FVector start = GetActorLocation();
	end.Z += 400.0f;
	//start.Z += 100.0F;
	if (GetWorld()->LineTraceSingleByChannel(hit, start, end, ECollisionChannel::ECC_Visibility))
	{
		AObstacle* obstacle = Cast<AObstacle>(hit.Actor);
		if (obstacle)
		{
			bTraversable = false;
		}
	}

	//SetActorHiddenInGame(true);
}

AGridManager* ATile::GetGridManager()
{
	return gridManager;
}
void ATile::SetGridManager(AGridManager* gridManager_)
{
	gridManager = gridManager_;
}

void ATile::Highlighted(int index_)
{
	if (bTraversable)
	{
		SetActorHiddenInGame(false);
		if (index_ == 0 || index_ == 1)
		{
			if (highlightedMaterial)
				mesh->SetMaterial(1, highlightedMaterial);
		}
		else if (index_ == 2)
		{
			//Heal material
		}
		else if (index_ == 3)
		{
			if (pathMaterial)
				mesh->SetMaterial(1, pathMaterial);
		}
		else if (index_ == 5) //When the player clicks on an enemy
		{
			if (pathMaterial)
				mesh->SetMaterial(1, pathMaterial);
		}

		HighlightedIndex = index_;
	}
}
void ATile::NotHighlighted()
{
	//SetActorHiddenInGame(true);
	if (originalMaterial)
		mesh->SetMaterial(1, originalMaterial);

	mesh->SetMaterial(2, mesh->GetMaterial(0));

	HighlightedIndex = -1;
	gCost = hCost = fCost = 0; //Reset fCost. The starting tile always has 0 fcost
}

int ATile::GetHighlighted()
{
	return HighlightedIndex;
}

void ATile::AddImmediateNeighbor(ATile* tile_)
{
	if (tile_ != nullptr && tile_ != this && !immediateNeighbors.Contains(tile_))
	{
		immediateNeighbors.Push(tile_);
		tile_->AddImmediateNeighbor(this);
	}
}

void ATile::AddDiagonalNeighbor(ATile* tile_)
{
	if (tile_ != nullptr && tile_ != this && !diagonalNeighbors.Contains(tile_))
	{
		diagonalNeighbors.Push(tile_);
		tile_->AddDiagonalNeighbor(this);
	}
}


TArray<ATile*> ATile::GetImmediateNeighbors()
{
	return immediateNeighbors;
}
TArray<ATile*> ATile::GetDiagonalNeighbors()
{
	return diagonalNeighbors;
}

bool ATile::GetTraversable()
{
	return bTraversable;
}

bool ATile::GetOccupied()
{
	return bOccupied;
}
void ATile::SetOccupied(bool value_)
{
	bOccupied = value_;
}


ATile* ATile::GetParentTile()
{
	return parentTile;
}
void ATile::SetParentTile(ATile* tile_)
{
	parentTile = tile_;
}


void ATile::CalculateHCost(ATile* tile_)
{
	if (tile_)
	{
		//hCost = FMath::Sqrt(FMath::Pow((GetActorLocation().X - tile_->GetActorLocation().X),2)
					 //     + FMath::Pow((GetActorLocation().Y - tile_->GetActorLocation().Y), 2));

		//Manhatten
		hCost = FMath::Abs(GetActorLocation().X - tile_->GetActorLocation().X)
			+ FMath::Abs(GetActorLocation().Y - tile_->GetActorLocation().Y);
	}
}

void ATile::HighlightPath()
{
	if (pathMaterial)
		mesh->SetMaterial(1, pathMaterial);
}

void ATile::HighlightNeighbor()
{

	for (int i = 0; i < immediateNeighbors.Num(); i++)
	{
		immediateNeighbors[i]->HighlightPath();
	}

	for (int i = 0; i < diagonalNeighbors.Num(); i++)
	{
		diagonalNeighbors[i]->HighlightPath();
	}
}
