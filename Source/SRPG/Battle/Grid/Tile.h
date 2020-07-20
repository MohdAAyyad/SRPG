// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UCLASS()
class SRPG_API ATile : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Tile")
		USceneComponent* root;

	UPROPERTY(EditAnywhere, Category = "Tile")
		UStaticMeshComponent* mesh;

	UPROPERTY(EditAnywhere, Category = "Tile")
		UMaterialInterface* originalMaterial;

	UPROPERTY(EditAnywhere, Category = "Tile")
		UMaterialInterface* highlightedMaterial;

	UPROPERTY(EditAnywhere, Category = "Tile")
		UMaterialInterface* pathMaterial;


	class AGridManager* gridManager;
	int HighlightedIndex;//0 move, 1 attack, 2 heal

	//Need different arrays for different neighbors to make cost calculations easier
	TArray<ATile*> immediateNeighbors;
	TArray<ATile*> diagonalNeighbors;
	ATile* parentTile; //The tile from where we evaluated this tile's costs
	bool bTraversable;

public:
	class AGridManager* GetGridManager();
	void SetGridManager(class AGridManager* gridManager_);
	void Highlighted(int index_);
	void NotHighlighted();
	int  GetHighlighted();
	void AddImmediateNeighbor(ATile* tile_);
	void AddDiagonalNeighbor(ATile* tile_);

	bool GetTraversable();

	TArray<ATile*> GetImmediateNeighbors();
	TArray<ATile*> GetDiagonalNeighbors();
	ATile* GetParentTile();
	void SetParentTile(ATile*);

	int gCost;
	int hCost;
	int fCost;

	void CalculateHCost(ATile* tile_);

	void HighlightPath();
	void HighlightNeighbor();
};
