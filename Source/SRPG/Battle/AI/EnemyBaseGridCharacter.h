// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/GridCharacter.h"
#include "EnemyBaseGridCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API AEnemyBaseGridCharacter : public AGridCharacter
{
	GENERATED_BODY()
protected:
	AEnemyBaseGridCharacter();
	class AAIManager* aiManager;
	class ATile* targetTile;
	class APlayerGridCharacter* targetPlayer;
	class AGridManager* gridManager;
	UPROPERTY(EditAnywhere, Category = "Detection")
		class UBoxComponent* detectionRadius;

	//Enemies move to this tile at the beginning
	UPROPERTY(EditAnywhere, Category = "Targeting")
		int rowIndexOfDeploymentTargetTile;
	UPROPERTY(EditAnywhere, Category = "Targeting")
		int offsetOfDeploymentTargetTile;
	UPROPERTY(EditAnywhere, Category = "Targeting")
		int attackRange; //PLACEHOLDER

public:
	void SetManagers(AAIManager* ref_, AGridManager* gref_);
	void PickATargetTile();
	void MoveToTarget();
};
