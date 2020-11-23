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
	UPROPERTY(EditAnywhere, Category = "Tile")
		UMaterialInterface* skillsMaterial;
	UPROPERTY(EditAnywhere, Category = "Tile")
		UMaterialInterface* itemMaterial;


	class AGridManager* gridManager;
	UPROPERTY(Replicated)
		int HighlightedIndex;//0 move, 1 attack, 2 heal 3 deplyment 4 reachable by enemy
	int previousHighlightIndex;

	//Need different arrays for different neighbors to make cost calculations easier
	TArray<ATile*> immediateNeighbors;
	TArray<ATile*> diagonalNeighbors;
	ATile* parentTile; //The tile from where we evaluated this tile's costs
	bool bTraversable;
	bool bOccupied; //If the tile is used by an enemy or a player, it becomes occupied


	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

public:
	class AGridManager* GetGridManager();
	void SetGridManager(class AGridManager* gridManager_);

	UFUNCTION(Server, Reliable)
		void Highlighted(int index_);
		void Highlighted_Implementation(int index_);

	void GoBackToPreviousHighlight();

	UFUNCTION(Server, Reliable)
		void NotHighlighted();
		void NotHighlighted_Implementation();

	int  GetHighlighted();
	void AddImmediateNeighbor(ATile* tile_);
	void AddDiagonalNeighbor(ATile* tile_);

	bool GetTraversable();
	bool GetOccupied();
	void SetOccupied(bool value_);

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

	class AGridCharacter* GetMyGridCharacter();

	void NightBlues();
};
