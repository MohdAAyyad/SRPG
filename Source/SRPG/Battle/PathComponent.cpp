// Fill out your copyright notice in the Description page of Project Settings.


#include "PathComponent.h"
#include "Grid/Tile.h"
#include "Definitions.h"

// Sets default values for this component's properties
UPathComponent::UPathComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	currentTile = nullptr;
	targetTile = nullptr;

	rowSpeed = 5;
	depth = 5;
	// ...
}

void UPathComponent::SetTargetTile(ATile* tile_)
{
	targetTile = tile_;

	ChangeTargetTileIfItsOccupied();
}

void UPathComponent::SetCurrentTile(ATile* tile_)
{
	currentTile = tile_;
	currentTile->SetOccupied(false);
}

TArray<FVector> UPathComponent::GetPathToTargetTile(int highlightIndex_)
{
	TArray<ATile*> open;
	TArray<ATile*> closed;
	TArray<FVector> path;

	if (movementPath.Num() > 0)
		movementPath.Empty();

	ATile* currentNode = nullptr;

	bool bFoundTarget = false;
	open.Push(currentTile);

	while (open.Num() > 0)
	{
		currentNode = GetTileWithMinFCost(open);

		if (currentNode)
		{
			open.Remove(currentNode); //Node has been visited

			if (currentNode == targetTile) //We've found the goal, get out
				break;

			closed.Push(currentNode); //The current node has been visited

			//Update parent nodes for immediate neighbors
			for (int i = 0; i < currentNode->GetImmediateNeighbors().Num(); i++)
			{
				if (currentNode->GetImmediateNeighbors()[i] != nullptr)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Checking hINdex %d"), currentNode->GetImmediateNeighbors()[i]->GetHighlighted());
					if (closed.Contains(currentNode->GetImmediateNeighbors()[i]) || (currentNode->GetImmediateNeighbors()[i]->GetHighlighted()!= highlightIndex_ && highlightIndex_ != -1)) //If the neighbor has already been visited or is not highlighted, move on to the next one. If the Hindex is -1 then this function is being called by an enemy and since the target tile can be non-highighlighted we need to negate this check, otherwise, we'll neverfind a path
						continue;

					currentNode->GetImmediateNeighbors()[i]->SetParentTile(currentNode); // Update the parent of the tile
					if (currentNode->gCost + 10 < currentNode->GetImmediateNeighbors()[i]->gCost) //Update the gcost of the neighbor only if the new gcost is smaller than the previous one
						currentNode->GetImmediateNeighbors()[i]->gCost = currentNode->gCost + 10; //Distance between node and imm neighbor is assumed 10
					currentNode->GetImmediateNeighbors()[i]->CalculateHCost(targetTile);
					currentNode->GetImmediateNeighbors()[i]->fCost = currentNode->GetImmediateNeighbors()[i]->gCost + currentNode->GetImmediateNeighbors()[i]->hCost;

					if (!open.Contains(currentNode->GetImmediateNeighbors()[i])
						&& !DoesListHaveALowerFCost(open, currentNode->GetImmediateNeighbors()[i]->fCost) //If we already have a tile in the open list that has a lower FCost, there's no need to bother with this tile
						&& currentNode->GetImmediateNeighbors()[i]->GetTraversable()
						)
						open.Push(currentNode->GetImmediateNeighbors()[i]);
				}


			}

			//Same as before but for diagonal neighbors
			//Update parent for diagonal neighbors 
			for (int d = 0; d < currentNode->GetDiagonalNeighbors().Num(); d++)
			{
				if (currentNode->GetDiagonalNeighbors()[d] != nullptr)
				{
					if (closed.Contains(currentNode->GetDiagonalNeighbors()[d]) || (currentNode->GetDiagonalNeighbors()[d]->GetHighlighted() != highlightIndex_ && highlightIndex_ != -1))
						continue;

					currentNode->GetDiagonalNeighbors()[d]->SetParentTile(currentNode);
					if (currentNode->gCost + 14 < currentNode->GetDiagonalNeighbors()[d]->gCost)
						currentNode->GetDiagonalNeighbors()[d]->gCost = currentNode->gCost + 14; //Distance between node and imm neighbor is assumed 10
					currentNode->GetDiagonalNeighbors()[d]->CalculateHCost(targetTile);
					currentNode->GetDiagonalNeighbors()[d]->fCost = currentNode->GetDiagonalNeighbors()[d]->gCost + currentNode->GetDiagonalNeighbors()[d]->hCost;

					if (!open.Contains(currentNode->GetDiagonalNeighbors()[d])
						&& !DoesListHaveALowerFCost(open, currentNode->GetDiagonalNeighbors()[d]->fCost)
						&& currentNode->GetDiagonalNeighbors()[d]->GetTraversable()
						)
						open.Push(currentNode->GetDiagonalNeighbors()[d]);
				}

			}
		}
	}

	UpdateMovementPath(targetTile);

	for (int i = 0; i < movementPath.Num(); i++)
	{
		path.Push(movementPath[i]->GetActorLocation());
	}
	return path;
}

ATile* UPathComponent::GetTileWithMinFCost(TArray<ATile*>& tiles_)
{
	int min = 10000000;
	ATile* minTile = nullptr;
	if (tiles_.Num() > 0)
	{
		if (tiles_[0] != nullptr)
		{
			min = tiles_[0]->fCost;
			minTile = tiles_[0];
		}
		for (int i = 0; i < tiles_.Num(); i++)
		{
			if (tiles_[i] != nullptr)
			{
				if (tiles_[i]->fCost < min)
				{
					min = tiles_[i]->fCost;
					minTile = tiles_[i];
				}
				else if (tiles_[i]->fCost == min && tiles_[i]->hCost < minTile->hCost)
				{
					min = tiles_[i]->fCost;
					minTile = tiles_[i];
				}
			}
		}
	}

	return minTile;
}

void UPathComponent::UpdateMovementPath(ATile* tile_)
{
	if (tile_)
	{
		if (tile_->GetParentTile() != nullptr && !movementPath.Contains(tile_))
		{
			movementPath.Push(tile_);
			if (tile_->GetParentTile() != currentTile)
				UpdateMovementPath(tile_->GetParentTile());
		}
	}
}

bool UPathComponent::DoesListHaveALowerFCost(TArray<ATile*>& list_, int fCost_)
{
	if (list_.Num() > 0)
	{
		int min = fCost_;

		for (int i = 0; i < list_.Num(); i++)
		{
			if (list_[i]->fCost < min)
				return true;
		}
	}
	return false;
}

void UPathComponent::UpdateSpeed(int spd_)
{
	//UE_LOG(LogTemp, Warning, TEXT("Speed %d"), spd_);
	//Diamond shape is the default for everyone
	rowSpeed = spd_;
	depth = rowSpeed + 1;
}

int UPathComponent::GetRowSpeed()
{
	return rowSpeed;
}
int UPathComponent::GetDepth()
{
	return depth;
}

TArray<ATile*> UPathComponent::GetMovementPath()
{
	return movementPath;
}

void UPathComponent::ChangeTargetTileIfItsOccupied()
{
	if (targetTile)
	{
		if (targetTile->GetOccupied())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Target ummm yeahhh"));
			targetTile = targetTile->GetParentTile();
			ChangeTargetTileIfItsOccupied();
		}
	}
}

void UPathComponent::AdjustPath(ATile* tile_, TArray<FVector>& move_)
{
	//Make sure your target tile is not occupied
	if (tile_)
	{
		if (tile_->GetOccupied())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Tile ummm yeahhh"));
			tile_->NotHighlighted();
			tile_ = tile_->GetParentTile();
			if (move_.Num() > 0)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Removing from move_"));
				move_.RemoveAt(0);
			}
			AdjustPath(tile_, move_);
		}
		else
		{ //The tile we found is not occupied. Mark it as such so no other enemy goes to it.
			tile_->SetOccupied(true);
		}
	}
}