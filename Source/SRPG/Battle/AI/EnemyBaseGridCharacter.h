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
	void BeginPlay() override;
	class AAIManager* aiManager;
	class ATile* targetTile;
	class APlayerGridCharacter* targetPlayer;
	class AGridManager* gridManager;
	UPROPERTY(EditAnywhere, Category = "Detection")
		class UBoxComponent* detectionRadius;

	class ABattleController* btlCtrl;


	//Enemies move to this tile at the beginning
	UPROPERTY(EditAnywhere, Category = "Targeting")
		int rowIndexOfDeploymentTargetTile;
	UPROPERTY(EditAnywhere, Category = "Targeting")
		int offsetOfDeploymentTargetTile;
	UPROPERTY(EditAnywhere, Category = "Targeting")
		int attackRange; //PLACEHOLDER

	UFUNCTION()
		void DetectPlayer(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult &sweepResult_);

public:
	void SetManagers(AAIManager* ref_, AGridManager* gref_);
	void PickMovementDestination();
	void EnableDetectionCollision();
	void ExecuteChosenAttack();
	virtual void Selected() override;
	virtual void NotSelected() override;
	virtual void ActivateWeaponAttack() override;
	void MoveAccordingToPath() override;
};
