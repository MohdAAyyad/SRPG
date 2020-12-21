// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"
#include "Tile.h"
#include "Engine/World.h"
#include "Definitions.h"
#include "Obstacle.h"

// Sets default values
AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = false;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;
	rowsNum = 5;
	columnsNum = 5;
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
			rowTileLoc = FVector(r*TILE_SIZE + GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
			ATile* tiler = GetWorld()->SpawnActor<ATile>(tileRef, rowTileLoc, FRotator::ZeroRotator);
			tiler->SetGridManager(this);
			rowTiles.Push(tiler);
			for (int c = 1; c < columnsNum; c++) //We start at 1 because the row tiles have already created one column
			{
				columnTileLoc = FVector(rowTileLoc.X, c*TILE_SIZE + rowTileLoc.Y, rowTileLoc.Z);

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

void AGridManager::UpdateCurrentTile(ATile* tile_, int rowSpeed_, int depth_,int index_, int pure_)
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
		if (pure_ == 0) //False
		{
			if (index_ == TILE_ATK || index_ == TILE_SKL || index_ == TILE_ENMA) //Non-pure attack tiles get do not highlight the tiles beyond an obstacles
			{
				HighlightAttackTiles(rowSpeed_, depth_, index_);
			}
			else
			{
				HighlightTiles(rowSpeed_, depth_, index_);//Non-pure Movement tiles can highlight the tiles beyond obstacle
			}
		}
		else
		{
			HighlightTilesPure(rowSpeed_, depth_, index_);//Pure attack and movement tiles do not get highlighted beyond obstacles.
		}
	}
}

void AGridManager::HighlightTiles(int rowSpeed_, int depth_, int index_)
{
	UE_LOG(LogTemp, Warning, TEXT("Highlight Tiles Index %d"), index_);
	int localDepth = depth_;
	columnOffset = tileIndexInColumn - ConvertRowTocolumn(tileIndexInRows);
	//Rows going upwards
	for (int i = tileIndexInRows; i < tileIndexInRows + rowSpeed_ + 1 && i < rowsNum; i++)
	{
		tileIndexInColumn = ConvertRowTocolumn(i) + columnOffset;
		if (tileIndexInColumn < columnTiles.Num())
		{
			if (columnTiles[tileIndexInColumn] != nullptr)
			{
				//if (columnTiles[tileIndexInColumn]->GetTraversable()) //If any of the tiles going upwards is non-traversable, then the next tile by default is non-traversable
				//{
					columnTiles[tileIndexInColumn]->Highlighted(index_);
					highlightedTiles.Push(columnTiles[tileIndexInColumn]);
			//	}
				//else
			//	{
			//		break;
			//	}
			}
		}
	}
	for (int i = tileIndexInRows; i < tileIndexInRows + rowSpeed_ + 1 && i < rowsNum; i++)
	{
		//Get the columne index of the tile
		tileIndexInColumn = ConvertRowTocolumn(i) + columnOffset;
		if (tileIndexInColumn < columnTiles.Num())
		{
			if (columnTiles[tileIndexInColumn] != nullptr)
			{
				//if (columnTiles[tileIndexInColumn]->GetHighlighted() == index_) //Make sure it's highlighted before highlighting the tiles on its right and left
				//{
					for (int d = 0; d < localDepth; d++)
					{
						//If it's on the same row, that's fine
						if (ConvertColumnToRow(tileIndexInColumn + d) == i)
						{
							if (tileIndexInColumn + d > 0 && tileIndexInColumn + d < columnTiles.Num())
							{
								//if (columnTiles[tileIndexInColumn + d - 1]->GetTraversable()) //Is the one to my left traversable?
								//{
									columnTiles[tileIndexInColumn + d]->Highlighted(index_);
									highlightedTiles.Push(columnTiles[tileIndexInColumn + d]);
								//}
								//else
							//	{
								//	break; //Otherwise, don't bother with the tiles on my left
							//	}
							}
						}
						if (ConvertColumnToRow(tileIndexInColumn - d) == i) // Right tiles
						{
							if (tileIndexInColumn - d > 0 && tileIndexInColumn - d + 1 < columnTiles.Num())
							{
								//if (columnTiles[tileIndexInColumn - d + 1]->GetTraversable())
								//{
									columnTiles[tileIndexInColumn - d]->Highlighted(index_);
									highlightedTiles.Push(columnTiles[tileIndexInColumn - d]);
								//}
								//else
								//{
									//break;
								//}
							}
						}
					}
				//}
			}

		}
		localDepth--;
	}
	localDepth = depth_;

	//Repeat the same process for rows going downwards

	for (int j = tileIndexInRows; j >= tileIndexInRows - rowSpeed_ && j >= 0; j--)
	{
		tileIndexInColumn = ConvertRowTocolumn(j) + columnOffset;

		if (tileIndexInColumn < columnTiles.Num())
		{
			if (columnTiles[tileIndexInColumn] != nullptr)
			{
				//if (columnTiles[tileIndexInColumn]->GetTraversable())
				//{
					columnTiles[tileIndexInColumn]->Highlighted(index_);
					highlightedTiles.Push(columnTiles[tileIndexInColumn]);
				//}
				//else
				//{
				//	break;
				//}
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
				//if (columnTiles[tileIndexInColumn]->GetHighlighted() == index_)
				//{
					for (int d = 0; d < localDepth; d++)
					{
						//If it's on the same row, that's fine
						if (ConvertColumnToRow(tileIndexInColumn + d) == j)
						{
							if (tileIndexInColumn + d > 0 && tileIndexInColumn + d < columnTiles.Num())
							{
								//if (columnTiles[tileIndexInColumn + d - 1]->GetTraversable())
								//{
									columnTiles[tileIndexInColumn + d]->Highlighted(index_);
									highlightedTiles.Push(columnTiles[tileIndexInColumn + d]);
								//}
								//else
								//{
								//	break;
								//}
							}
						}
						if (ConvertColumnToRow(tileIndexInColumn - d) == j)
						{
							if (tileIndexInColumn - d > 0 && tileIndexInColumn - d + 1 < columnTiles.Num())
							{
								//if (columnTiles[tileIndexInColumn - d + 1]->GetTraversable())
								//{
									columnTiles[tileIndexInColumn - d]->Highlighted(index_);
									highlightedTiles.Push(columnTiles[tileIndexInColumn - d]);
								//}
								//else
								//{
								//	break;
								//}
							}
						}
					}
				//}
			}
		}
		localDepth--;
	}
}

void AGridManager::HighlightAttackTiles(int rowSpeed_, int depth_, int index_)
{
	int localDepth = depth_;
	columnOffset = tileIndexInColumn - ConvertRowTocolumn(tileIndexInRows);
	//Rows going upwards
	for (int i = tileIndexInRows; i < tileIndexInRows + rowSpeed_ + 1 && i < rowsNum; i++)
	{
		tileIndexInColumn = ConvertRowTocolumn(i) + columnOffset;
		if (tileIndexInColumn < columnTiles.Num())
		{
			if (columnTiles[tileIndexInColumn] != nullptr)
			{
				if (columnTiles[tileIndexInColumn]->GetTraversable()) //If any of the tiles going upwards is non-traversable, then the next tile by default is non-traversable
				{
					columnTiles[tileIndexInColumn]->Highlighted(index_);
					highlightedTiles.Push(columnTiles[tileIndexInColumn]);
				}
				else if(index_ != TILE_ENM && index_!=TILE_ENMA && index_ != TILE_ENMH) //Enemies' awareness of obstacles is that they block the path so the tiles must not be highlighted
				{ 
					//Get the obstacle that's blocking this tile
					AObstacle* obstacle_ = columnTiles[tileIndexInColumn]->GetMyObstacle();
					if (obstacle_)
					{
						//Highlight all its tiles. We need this to be able to target the obstacle
						TArray<ATile*> obstructedTiles = obstacle_->GetObstructedTiles();

						for (int i = 0; i < obstructedTiles.Num(); i++)
						{
							obstructedTiles[i]->Highlighted(index_);
							if (!highlightedTiles.Contains(obstructedTiles[i]))
								highlightedTiles.Push(obstructedTiles[i]);
						}
					}
					break;
				}
				else
				{
					break;
				}
			}
		}
	}
	for (int i = tileIndexInRows; i < tileIndexInRows + rowSpeed_ + 1 && i < rowsNum; i++)
	{
		//Get the columne index of the tile
		tileIndexInColumn = ConvertRowTocolumn(i) + columnOffset;
		if (tileIndexInColumn < columnTiles.Num())
		{
			if (columnTiles[tileIndexInColumn] != nullptr)
			{
				if (columnTiles[tileIndexInColumn]->GetHighlighted() == index_) //Make sure it's highlighted before highlighting the tiles on its right and left
				{
					for (int d = 0; d < localDepth; d++)
					{
						//If it's on the same row, that's fine
						if (ConvertColumnToRow(tileIndexInColumn + d) == i)
						{
							if (tileIndexInColumn + d > 0 && tileIndexInColumn + d < columnTiles.Num())
							{
								if (columnTiles[tileIndexInColumn + d - 1]->GetTraversable()) //Is the one to my left traversable?
								{
								columnTiles[tileIndexInColumn + d]->Highlighted(index_);
								highlightedTiles.Push(columnTiles[tileIndexInColumn + d]);
								}
								else if (index_ != TILE_ENM && index_ != TILE_ENMA && index_ != TILE_ENMH) //Enemies' awareness of obstacles is that they block the path so the tiles must not be highlighted
								{

									//Get the obstacle that's blocking this tile
									AObstacle* obstacle_ = columnTiles[tileIndexInColumn + d - 1]->GetMyObstacle();
									if (obstacle_)
									{
										//Highlight all its tiles. We need this to be able to target the obstacle
										TArray<ATile*> obstructedTiles = obstacle_->GetObstructedTiles();

										for (int i = 0; i < obstructedTiles.Num(); i++)
										{
											obstructedTiles[i]->Highlighted(index_);
											if (!highlightedTiles.Contains(obstructedTiles[i]))
												highlightedTiles.Push(obstructedTiles[i]);
										}
									}
									break;
								}
								else //Enemy tiles
								{
									break;
								}
							}
						}
						if (ConvertColumnToRow(tileIndexInColumn - d) == i) // Right tiles
						{
							if (tileIndexInColumn - d > 0 && tileIndexInColumn - d + 1 < columnTiles.Num())
							{
								if (columnTiles[tileIndexInColumn - d + 1]->GetTraversable())
								{
								columnTiles[tileIndexInColumn - d]->Highlighted(index_);
								highlightedTiles.Push(columnTiles[tileIndexInColumn - d]);
								}
								else if (index_ != TILE_ENM && index_ != TILE_ENMA && index_ != TILE_ENMH) //Enemies' awareness of obstacles is that they block the path so the tiles must not be highlighted
								{
									//Get the obstacle that's blocking this tile
									AObstacle* obstacle_ = columnTiles[tileIndexInColumn - d + 1]->GetMyObstacle();
									if (obstacle_)
									{
										//Highlight all its tiles. We need this to be able to target the obstacle
										TArray<ATile*> obstructedTiles = obstacle_->GetObstructedTiles();

										for (int i = 0; i < obstructedTiles.Num(); i++)
										{
											obstructedTiles[i]->Highlighted(index_);
											if (!highlightedTiles.Contains(obstructedTiles[i]))
												highlightedTiles.Push(obstructedTiles[i]);
										}
									}
									break;
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

	//Repeat the same process for rows going downwards

	for (int j = tileIndexInRows; j >= tileIndexInRows - rowSpeed_ && j >= 0; j--)
	{
		tileIndexInColumn = ConvertRowTocolumn(j) + columnOffset;

		if (tileIndexInColumn < columnTiles.Num())
		{
			if (columnTiles[tileIndexInColumn] != nullptr)
			{
				if (columnTiles[tileIndexInColumn]->GetTraversable())
				{
					columnTiles[tileIndexInColumn]->Highlighted(index_);
					highlightedTiles.Push(columnTiles[tileIndexInColumn]);
				}
				else if (index_ != TILE_ENM && index_ != TILE_ENMA && index_ != TILE_ENMH) //Enemies' awareness of obstacles is that they block the path so the tiles must not be highlighted
				{
					//Get the obstacle that's blocking this tile
					AObstacle* obstacle_ = columnTiles[tileIndexInColumn]->GetMyObstacle();
					if (obstacle_)
					{
						//Highlight all its tiles. We need this to be able to target the obstacle
						TArray<ATile*> obstructedTiles = obstacle_->GetObstructedTiles();

						for (int i = 0; i < obstructedTiles.Num(); i++)
						{
							obstructedTiles[i]->Highlighted(index_);
							if (!highlightedTiles.Contains(obstructedTiles[i]))
								highlightedTiles.Push(obstructedTiles[i]);
						}
					}
					break;
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
				if (columnTiles[tileIndexInColumn]->GetHighlighted() == index_)
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
								columnTiles[tileIndexInColumn + d]->Highlighted(index_);
								highlightedTiles.Push(columnTiles[tileIndexInColumn + d]);
								}
								else if (index_ != TILE_ENM && index_ != TILE_ENMA && index_ != TILE_ENMH) //Enemies' awareness of obstacles is that they block the path so the tiles must not be highlighted
								{
									//Get the obstacle that's blocking this tile
									AObstacle* obstacle_ = columnTiles[tileIndexInColumn + d - 1]->GetMyObstacle();
									if (obstacle_)
									{
										//Highlight all its tiles. We need this to be able to target the obstacle
										TArray<ATile*> obstructedTiles = obstacle_->GetObstructedTiles();

										for (int i = 0; i < obstructedTiles.Num(); i++)
										{
											obstructedTiles[i]->Highlighted(index_);
											if (!highlightedTiles.Contains(obstructedTiles[i]))
												highlightedTiles.Push(obstructedTiles[i]);
										}
									}
									break;
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
								columnTiles[tileIndexInColumn - d]->Highlighted(index_);
								highlightedTiles.Push(columnTiles[tileIndexInColumn - d]);
								}
								else if (index_ != TILE_ENM && index_ != TILE_ENMA && index_ != TILE_ENMH) //Enemies' awareness of obstacles is that they block the path so the tiles must not be highlighted
								{
									//Get the obstacle that's blocking this tile
									AObstacle* obstacle_ = columnTiles[tileIndexInColumn - d + 1]->GetMyObstacle();
									if (obstacle_)
									{
										//Highlight all its tiles. We need this to be able to target the obstacle
										TArray<ATile*> obstructedTiles = obstacle_->GetObstructedTiles();

										for (int i = 0; i < obstructedTiles.Num(); i++)
										{
											obstructedTiles[i]->Highlighted(index_);
											if (!highlightedTiles.Contains(obstructedTiles[i]))
												highlightedTiles.Push(obstructedTiles[i]);
										}
									}
									break;
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


void AGridManager::HighlightTilesPure(int rowSpeed_, int depth_, int index_)
{
	columnOffset = tileIndexInColumn - ConvertRowTocolumn(tileIndexInRows);
	//Rows going upwards
	for (int i = tileIndexInRows; i < tileIndexInRows + rowSpeed_ + 1 && i < rowsNum; i++)
	{
		tileIndexInColumn = ConvertRowTocolumn(i) + columnOffset;
		if (tileIndexInColumn < columnTiles.Num())
		{
			if (columnTiles[tileIndexInColumn] != nullptr)
			{
				if (columnTiles[tileIndexInColumn]->GetTraversable()) //If any of the tiles going upwards is non-traversable, then the next tile by default is non-traversable
				{
					columnTiles[tileIndexInColumn]->Highlighted(index_);
					highlightedTiles.Push(columnTiles[tileIndexInColumn]);
				}
				else if (index_ != TILE_ENM && index_ != TILE_ENMA && index_ != TILE_ENMH) //Enemies' awareness of obstacles is that they block the path so the tiles must not be highlighted
				{
					//Get the obstacle that's blocking this tile
					AObstacle* obstacle_ = columnTiles[tileIndexInColumn]->GetMyObstacle();
					if (obstacle_)
					{
						//Highlight all its tiles. We need this to be able to target the obstacle
						TArray<ATile*> obstructedTiles = obstacle_->GetObstructedTiles();

						for (int i = 0; i < obstructedTiles.Num(); i++)
						{
							obstructedTiles[i]->Highlighted(index_);
							if (!highlightedTiles.Contains(obstructedTiles[i]))
								highlightedTiles.Push(obstructedTiles[i]);
						}
					}
					break;
				}
				else
				{
					break;
				}
			}
		}
	}
	//Get the column index of the origin tile. We won't need to loop here as we won't go diagonally
	tileIndexInColumn = ConvertRowTocolumn(tileIndexInRows) + columnOffset;
	if (tileIndexInColumn < columnTiles.Num())
	{
		if (columnTiles[tileIndexInColumn] != nullptr)
		{
			if (columnTiles[tileIndexInColumn]->GetHighlighted() == index_) //Make sure it's highlighted before highlighting the tiles on its right and left
			{
				for (int d = 0; d < depth_; d++)
				{
					//If it's on the same row, that's fine
					if (ConvertColumnToRow(tileIndexInColumn + d) == tileIndexInRows)
					{
						if (tileIndexInColumn + d > 0 && tileIndexInColumn + d < columnTiles.Num())
						{
							if (columnTiles[tileIndexInColumn + d - 1]->GetTraversable()) //Is the one to my left traversable?
							{
									columnTiles[tileIndexInColumn + d]->Highlighted(index_);
									highlightedTiles.Push(columnTiles[tileIndexInColumn + d]);
							}
							else if (index_ != TILE_ENM && index_ != TILE_ENMA && index_ != TILE_ENMH) //Enemies' awareness of obstacles is that they block the path so the tiles must not be highlighted
							{
								//Get the obstacle that's blocking this tile
								AObstacle* obstacle_ = columnTiles[tileIndexInColumn + d - 1]->GetMyObstacle();
								if (obstacle_)
								{
									//Highlight all its tiles. We need this to be able to target the obstacle
									TArray<ATile*> obstructedTiles = obstacle_->GetObstructedTiles();

									for (int i = 0; i < obstructedTiles.Num(); i++)
									{
										obstructedTiles[i]->Highlighted(index_);
										if (!highlightedTiles.Contains(obstructedTiles[i]))
											highlightedTiles.Push(obstructedTiles[i]);
									}
								}
								break;
							}
							else
							{
								break;
							}
						}
					}
					if (ConvertColumnToRow(tileIndexInColumn - d) == tileIndexInRows) // Right tiles
					{
						if (tileIndexInColumn - d > 0 && tileIndexInColumn - d + 1 < columnTiles.Num())
						{
							if (columnTiles[tileIndexInColumn - d + 1]->GetTraversable())
							{
								columnTiles[tileIndexInColumn - d]->Highlighted(index_);
								highlightedTiles.Push(columnTiles[tileIndexInColumn - d]);
							}
							else if (index_ != TILE_ENM && index_ != TILE_ENMA && index_ != TILE_ENMH) //Enemies' awareness of obstacles is that they block the path so the tiles must not be highlighted
							{
								//Get the obstacle that's blocking this tile
								AObstacle* obstacle_ = columnTiles[tileIndexInColumn - d + 1]->GetMyObstacle();
								if (obstacle_)
								{
									//Highlight all its tiles. We need this to be able to target the obstacle
									TArray<ATile*> obstructedTiles = obstacle_->GetObstructedTiles();

									for (int i = 0; i < obstructedTiles.Num(); i++)
									{
										obstructedTiles[i]->Highlighted(index_);
										if (!highlightedTiles.Contains(obstructedTiles[i]))
											highlightedTiles.Push(obstructedTiles[i]);
									}
								}
								break;
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


	//Highlight the rows going down

	for (int j = tileIndexInRows; j >= tileIndexInRows - rowSpeed_ && j >= 0; j--)
	{
		tileIndexInColumn = ConvertRowTocolumn(j) + columnOffset;

		if (tileIndexInColumn < columnTiles.Num())
		{
			if (columnTiles[tileIndexInColumn] != nullptr)
			{
				if (columnTiles[tileIndexInColumn]->GetTraversable())
				{
					columnTiles[tileIndexInColumn]->Highlighted(index_);
					highlightedTiles.Push(columnTiles[tileIndexInColumn]);
				}
				else if (index_ != TILE_ENM && index_ != TILE_ENMA && index_ != TILE_ENMH) //Enemies' awareness of obstacles is that they block the path so the tiles must not be highlighted
				{
					//Get the obstacle that's blocking this tile
					AObstacle* obstacle_ = columnTiles[tileIndexInColumn]->GetMyObstacle();
					if (obstacle_)
					{
						//Highlight all its tiles. We need this to be able to target the obstacle
						TArray<ATile*> obstructedTiles = obstacle_->GetObstructedTiles();

						for (int i = 0; i < obstructedTiles.Num(); i++)
						{
							obstructedTiles[i]->Highlighted(index_);
							if (!highlightedTiles.Contains(obstructedTiles[i]))
								highlightedTiles.Push(obstructedTiles[i]);
						}
					}
					break;
				}
				else
				{
					break;
				}
			}
		}
	}

	//No need to loop through the depth for downwards tiles as we are not going diagonally
}

void AGridManager::HighlightDeploymentTiles(int rowIndex_, int offset_, int rowSpeed_, int depth_)
{
	int columnTileIndex = ConvertRowTocolumn(rowIndex_) + offset_;
	if (columnTileIndex >= 0 && columnTileIndex < columnTiles.Num())
	{
		UpdateCurrentTile(columnTiles[columnTileIndex], rowSpeed_, depth_, TILE_DEP, 0);
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

TArray<ATile*> AGridManager::GetHighlightedTiles()
{
	return highlightedTiles;
}

ATile* AGridManager::GetTileFromRowAndOffset(int row_, int offset_)
{
	int columnTileIndex = ConvertRowTocolumn(row_) + offset_;
	if (columnTileIndex >= 0 && columnTileIndex < columnTiles.Num())
	{
		return columnTiles[columnTileIndex];
	}
	return nullptr;
}