// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PathComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SRPG_API UPathComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPathComponent();

protected:
		int rowSpeed;
		int depth;

	class ATile* originTile;
	ATile* currentTile;
	ATile* targetTile;
	TArray<ATile*> movementPath;

	ATile* GetTileWithMinFCost(TArray<ATile*>& tiles_);
	void ChangeTargetTileIfItsOccupied();

public:	
	void SetCurrentTile(ATile* tile_);
	void SetTargetTile(ATile* tile_);
	TArray<FVector> GetPath();
	TArray<ATile*> GetMovementPath();

	void UpdateMovementPath(ATile* tile_);
	bool DoesListHaveALowerFCost(TArray<ATile*>& list_, int fCost_);

	void UpdateSpeed(int spd_);
	int GetRowSpeed();
	int GetDepth();

	void AdjustPath(ATile* tile_, TArray<FVector>& move_);


		
};
