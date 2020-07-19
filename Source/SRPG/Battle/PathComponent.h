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
	UPROPERTY(EditAnywhere, Category = "Grid")
		int rowSpeed;
	UPROPERTY(EditAnywhere, Category = "Grid")
		int depth;

	class ATile* originTile;
	ATile* currentTile;
	ATile* targetTile;
	TArray<ATile*> movementPath;

	ATile* GetTileWithMinFCost(TArray<ATile*> tiles_);

public:	
	void SetCurrentTile(ATile* tile_);
	void SetTargetTile(ATile* tile_);
	TArray<FVector> GetPath();

	void UpdateMovementPath(ATile* tile_);
	bool DoesOpenListHaveALowerFCost(TArray<ATile*> list_, int fCost_);
	bool DoesClosedListHaveALowerFCost(TArray<ATile*> list_, int fCost_);

	int GetRowSpeed();
	int GetDepth();
		
};
