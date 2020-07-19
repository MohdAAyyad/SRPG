// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"
#include "Tile.h"
#include "Engine/World.h"

// Sets default values
AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = false;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;
	rowsNum = 5;
	columnsNum = 5;
	tileSize = 100.0f;

	rowTiles.Reserve(rowsNum);
	columnTiles.Reserve(columnsNum);
	tileIndexInColumn = 0;
	tileIndexInRows = 0;
	columnOffset = 0;

}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	FVector rowTileLoc;
	FVector columnTileLoc;
	int neighborCounter = 0; //The tile on the right most part of the grid does not add the next tile as a neighbor
	if (tileRef)
	{
		//The row tiles are the ones of the left most part of the grid. Everything else is a column tile
		//The row tiles act as anchors, and will not have any functionality in the game itself
		for (int r = 0; r < rowsNum; r++)
		{
			rowTileLoc = FVector(r*tileSize + GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
			ATile* tiler = GetWorld()->SpawnActor<ATile>(tileRef, rowTileLoc, FRotator::ZeroRotator);
			tiler->SetGridManager(this);
			rowTiles.Push(tiler);
			for (int c = 1; c < columnsNum; c++) //We start at 1 because the row tiles have already created one column
			{
				columnTileLoc = FVector(rowTileLoc.X, c*tileSize + rowTileLoc.Y, rowTileLoc.Z);

				ATile* tilec = GetWorld()->SpawnActor<ATile>(tileRef, columnTileLoc, FRotator::ZeroRotator);
				tilec->SetGridManager(this);
				columnTiles.Push(tilec);
			}
		}

		//Update neighbors in column tile (rows are only anchors)
		for (int i = 0; i < columnTiles.Num(); i++)
		{
			if (columnTiles[i] != nullptr)
			{
				neighborCounter++;

				//Add the tile to the right
				//Make sure we're not going off the grid and make sure that the right most tile does not add the next tile as a neighbor
				if (i + 1 < columnTiles.Num() && neighborCounter < columnsNum - 1)
				{
					if (columnTiles[i + 1] != nullptr)
					{
						columnTiles[i]->AddImmediateNeighbor(columnTiles[i + 1]);
					}
				}
				//Add the tile above the current tile
				if (i + columnsNum - 1 < columnTiles.Num())
				{
					if (columnTiles[i + columnsNum - 1] != nullptr)
					{
						columnTiles[i]->AddImmediateNeighbor(columnTiles[i + columnsNum - 1]);
					}
				}

				//Add diagonal tile to the right of the current tile
				if (i + columnsNum < columnTiles.Num() && neighborCounter < columnsNum - 1)
				{
					if (columnTiles[i + columnsNum] != nullptr)
					{
						columnTiles[i]->AddDiagonalNeighbor(columnTiles[i + columnsNum]);
					}
				}

				//Add diagonal tile to the left of the current tile
				if (i + columnsNum - 2 < columnTiles.Num() && neighborCounter > 1)
				{
					if (columnTiles[i + columnsNum - 2] != nullptr)
					{
						columnTiles[i]->AddDiagonalNeighbor(columnTiles[i + columnsNum - 2]);
					}
				}

				//Reached the right most tile, reset the neighbor counter
				if (neighborCounter == columnsNum - 1)
				{
					neighborCounter = 0;
				}
			}
		}
	}

}

void AGridManager::UpdateCurrentTile(ATile* tile_, int rowSpeed_, int depth_)
{
	if (tile_)
	{
		//Check if it's a row tile or a column tile
		if (columnTiles.Contains(tile_))
		{
			//Get the column tile index
			tileIndexInColumn = columnTiles.IndexOfByKey(tile_);
			//Get the row tile index
			tileIndexInRows = ConvertColumnToRow(tileIndexInColumn);
		}
		else if (rowTiles.Contains(tile_))
		{
			//Get the row tile index
			//UE_LOG(LogTemp, Warning, TEXT("Row Tile"));
			tileIndexInRows = rowTiles.IndexOfByKey(tile_);
			//Get the column tile index
			tileIndexInColumn = ConvertRowTocolumn(tileIndexInRows);
		}
		HighlightTiles(rowSpeed_, depth_);
	}
}

void AGridManager::HighlightTiles(int rowSpeed_, int depth_)
{
	int localDepth = depth_;
	columnOffset = tileIndexInColumn - ConvertRowTocolumn(tileIndexInRows);
	//Rows going upwards
	for (int i = tileIndexInRows; i < tileIndexInRows + rowSpeed_ && i < rowsNum; i++)
	{
		tileIndexInColumn = ConvertRowTocolumn(i) + columnOffset;
		if (tileIndexInColumn < columnTiles.Num())
		{
			if (columnTiles[tileIndexInColumn] != nullptr)
			{
				if (columnTiles[tileIndexInColumn]->GetTraversable()) //If any of the tiles going upwards is non-traversable, then the next tile by default is non-traversable
				{
					columnTiles[tileIndexInColumn]->Highlighted();
					highlightedTiles.Push(columnTiles[tileIndexInColumn]);
				}
				else
				{
					break;
				}
			}
		}
	}
	for (int i = tileIndexInRows; i < tileIndexInRows + rowSpeed_ && i < rowsNum; i++)
	{
		tileIndexInColumn = ConvertRowTocolumn(i) + columnOffset;
		if (tileIndexInColumn < columnTiles.Num())
		{
			if (columnTiles[tileIndexInColumn] != nullptr)
			{
				if (columnTiles[tileIndexInColumn]->GetHighlighted())
				{
					for (int d = 0; d < localDepth; d++)
					{
						//If it's on the same row, that's fine
						if (ConvertColumnToRow(tileIndexInColumn + d) == i)
						{
							//	UE_LOG(LogTemp, Warning, TEXT("tileIndexInColumn + d %d "), tileIndexInColumn + d);
							if (tileIndexInColumn + d > 0 && tileIndexInColumn + d < columnTiles.Num())
							{
								if (columnTiles[tileIndexInColumn + d - 1]->GetTraversable()) //Is the one to my left traversable?
								{
									columnTiles[tileIndexInColumn + d]->Highlighted();
									highlightedTiles.Push(columnTiles[tileIndexInColumn + d]);
								}
								else
								{
									break;
								}
							}
						}
						if (ConvertColumnToRow(tileIndexInColumn - d) == i)
						{
							//	UE_LOG(LogTemp, Warning, TEXT("tileIndexInColumn - d %d "), tileIndexInColumn - d);
							if (tileIndexInColumn - d > 0 && tileIndexInColumn - d + 1 < columnTiles.Num())
							{
								if (columnTiles[tileIndexInColumn - d + 1]->GetTraversable())
								{
									columnTiles[tileIndexInColumn - d]->Highlighted();
									highlightedTiles.Push(columnTiles[tileIndexInColumn - d]);
								}
								else
								{
									break;
								}
							}
						}
					}
				}
			}

		}
		localDepth--;
	}
	localDepth = depth_;

	//Rows going downwards

	for (int j = tileIndexInRows; j >= tileIndexInRows - rowSpeed_ && j >= 0; j--)
	{
		tileIndexInColumn = ConvertRowTocolumn(j) + columnOffset;

		if (tileIndexInColumn < columnTiles.Num())
		{
			if (columnTiles[tileIndexInColumn] != nullptr)
			{
				if (columnTiles[tileIndexInColumn]->GetTraversable())
				{
					columnTiles[tileIndexInColumn]->Highlighted();
					highlightedTiles.Push(columnTiles[tileIndexInColumn]);
				}
				else
				{
					break;
				}
			}
		}
	}

	for (int j = tileIndexInRows; j >= tileIndexInRows - rowSpeed_ && j >= 0; j--)
	{
		tileIndexInColumn = ConvertRowTocolumn(j) + columnOffset;

		if (tileIndexInColumn < columnTiles.Num())
		{
			if (columnTiles[tileIndexInColumn] != nullptr)
			{
				if (columnTiles[tileIndexInColumn]->GetHighlighted())
				{
					for (int d = 0; d < localDepth; d++)
					{
						//If it's on the same row, that's fine
						if (ConvertColumnToRow(tileIndexInColumn + d) == j)
						{
							if (tileIndexInColumn + d > 0 && tileIndexInColumn + d < columnTiles.Num())
							{
								if (columnTiles[tileIndexInColumn + d - 1]->GetTraversable())
								{
									columnTiles[tileIndexInColumn + d]->Highlighted();
									highlightedTiles.Push(columnTiles[tileIndexInColumn + d]);
								}
								else
								{
									break;
								}
							}
						}
						if (ConvertColumnToRow(tileIndexInColumn - d) == j)
						{
							if (tileIndexInColumn - d > 0 && tileIndexInColumn - d + 1 < columnTiles.Num())
							{
								if (columnTiles[tileIndexInColumn - d + 1]->GetTraversable())
								{
									columnTiles[tileIndexInColumn - d]->Highlighted();
									highlightedTiles.Push(columnTiles[tileIndexInColumn - d]);
								}
								else
								{
									break;
								}
							}
						}
					}
				}
			}
		}
		localDepth--;
	}
}

int AGridManager::ConvertRowTocolumn(int index_)
{
	//Under the assumption the grid is a square
	if (index_ != 0)
	{
		return (index_ * columnsNum - index_);
	}
	return 0;
}
int AGridManager::ConvertColumnToRow(int index_)
{
	//Under the assumption the grid is a square
	if (index_ != 0)
	{
		return (index_ / (columnsNum - 1));
	}
	return 0;
}

void AGridManager::ClearHighlighted()
{
	if (highlightedTiles.Num() > 0)
	{
		for (int i = 0; i < highlightedTiles.Num(); i++)
		{
			highlightedTiles[i]->NotHighlighted();
		}

		highlightedTiles.Empty();
	}
}
