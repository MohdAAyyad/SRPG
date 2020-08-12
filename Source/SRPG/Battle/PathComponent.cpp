// Fill out your copyright notice in the Description page of Project Settings.


#include "PathComponent.h"
#include "Grid/Tile.h"

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
}

TArray<FVector> UPathComponent::GetPath()
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
			open.Remove(currentNode);

			if (currentNode == targetTile) //We've found the goal, get out
				break;

			closed.Push(currentNode); //The current node has been visited

			//Update parent nodes for immediate neighbors
			for (int i = 0; i < currentNode->GetImmediateNeighbors().Num(); i++)
			{
				if (currentNode->GetImmediateNeighbors()[i] != nullptr)
				{
					if (closed.Contains(currentNode->GetImmediateNeighbors()[i])) //If the neighbor has already been visited, move on to the next one
						continue;

					currentNode->GetImmediateNeighbors()[i]->SetParentTile(currentNode); // Update the parent of the tile
					if (currentNode->gCost + 10 < currentNode->GetImmediateNeighbors()[i]->gCost) //Update the gcost of the neighbor only if the new gcost is smaller than the previous one
						currentNode->GetImmediateNeighbors()[i]->gCost = currentNode->gCost + 10; //Distance between node and imm neighbor is assumed 10
					currentNode->GetImmediateNeighbors()[i]->CalculateHCost(targetTile);
					currentNode->GetImmediateNeighbors()[i]->fCost = currentNode->GetImmediateNeighbors()[i]->gCost + currentNode->GetImmediateNeighbors()[i]->hCost;

					if (!open.Contains(currentNode->GetImmediateNeighbors()[i])
						&& !DoesClosedListHaveALowerFCost(open, currentNode->GetImmediateNeighbors()[i]->fCost)
						&& !DoesOpenListHaveALowerFCost(open, currentNode->GetImmediateNeighbors()[i]->fCost)
						&& currentNode->GetImmediateNeighbors()[i]->GetTraversable()
						//&& (currentNode->GetImmediateNeighbors()[i]->GetHighlighted()==0|| currentNode->GetImmediateNeighbors()[i]->GetHighlighted() == 4)
						//&& !currentNode->GetOccupied()
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
					if (closed.Contains(currentNode->GetDiagonalNeighbors()[d]))
						continue;

					currentNode->GetDiagonalNeighbors()[d]->SetParentTile(currentNode);
					if (currentNode->gCost + 14 < currentNode->GetDiagonalNeighbors()[d]->gCost)
						currentNode->GetDiagonalNeighbors()[d]->gCost = currentNode->gCost + 14; //Distance between node and imm neighbor is assumed 10
					currentNode->GetDiagonalNeighbors()[d]->CalculateHCost(targetTile);
					currentNode->GetDiagonalNeighbors()[d]->fCost = currentNode->GetDiagonalNeighbors()[d]->gCost + currentNode->GetDiagonalNeighbors()[d]->hCost;

					if (!open.Contains(currentNode->GetDiagonalNeighbors()[d])
						&& !DoesClosedListHaveALowerFCost(open, currentNode->GetDiagonalNeighbors()[d]->fCost)
						&& !DoesOpenListHaveALowerFCost(open, currentNode->GetDiagonalNeighbors()[d]->fCost)
						&& currentNode->GetDiagonalNeighbors()[d]->GetTraversable()
						//&& (currentNode->GetImmediateNeighbors()[d]->GetHighlighted() == 0 || currentNode->GetImmediateNeighbors()[d]->GetHighlighted() == 4)
						//&& !currentNode->GetOccupied()
						)
						open.Push(currentNode->GetDiagonalNeighbors()[d]);
				}

			}
		}
	}

	UpdateMovementPath(targetTile);
	//currentTile->HighlightPath();

	for (int i = 0; i < movementPath.Num(); i++)
	{
		//movementPath[i]->HighlightPath();
		path.Push(movementPath[i]->GetActorLocation());
	}
	return path;
}

ATile* UPathComponent::GetTileWithMinFCost(TArray<ATile*> tiles_)
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

bool UPathComponent::DoesOpenListHaveALowerFCost(TArray<ATile*> list_, int fCost_)
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
bool UPathComponent::DoesClosedListHaveALowerFCost(TArray<ATile*> list_, int fCost_)
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
	if (targetTile->GetOccupied())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Target ummm yeahhh"));
		targetTile = targetTile->GetParentTile();
		ChangeTargetTileIfItsOccupied();
	}
}

void UPathComponent::AdjustPath(ATile* tile_, TArray<FVector>& move_)
{
	if (tile_->GetOccupied())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Tile ummm yeahhh"));
		tile_ = tile_->GetParentTile();
		if (move_.Num() > 0)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Removing from move_"));
			move_.RemoveAt(0);
		}
		AdjustPath(tile_, move_);
	}
	else
	{
		tile_->SetOccupied(true);
	}
}