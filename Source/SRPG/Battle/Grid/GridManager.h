// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

UCLASS()
class SRPG_API AGridManager : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AGridManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Grid")
		USceneComponent* root;
	UPROPERTY(EditAnywhere, Category = "Grid")
		int rowsNum;
	UPROPERTY(EditAnywhere, Category = "Grid")
		int columnsNum;
	UPROPERTY(EditAnywhere, Category = "Grid")
		TSubclassOf<class ATile> tileRef;

	TArray<ATile*> rowTiles;
	TArray<ATile*> columnTiles;
	TArray<ATile*> highlightedTiles;
	int tileIndexInColumn;
	int tileIndexInRows;
	int columnOffset;

	int ConvertRowTocolumn(int index_);
	int ConvertColumnToRow(int index_);


public:
	void UpdateCurrentTile(ATile* tile_, int rowSpeed_, int depth_, int index_, int pure_);
	void ClearHighlighted();

	void HighlightTiles(int rowSpeed_, int depth_, int index_);
	void HighlightAttackTiles(int rowSpeed_, int depth_, int index_);
	void HighlightTilesPure(int rowSpeed_, int depth_, int index_); //Pure vertical and horizontal highlight
	void HighlightDeploymentTiles(int rowIndex_, int offset_, int rowSpeed_, int depth_);
	TArray<ATile*> GetHighlightedTiles();
	ATile* GetTileFromRowAndOffset(int row_, int offset_);
	void GetTilesWithinAttackRange(TArray<ATile*>& rangeTiles_, int range_, ATile* tile_); //Called by enemies to get the tiles that are within their attack range of their target
};