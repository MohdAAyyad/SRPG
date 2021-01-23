// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElementalHazard.h"
#include "ObstaclesManager.generated.h"

UCLASS()
class SRPG_API AObstaclesManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AObstaclesManager();

protected:
	virtual void BeginPlay() override;

	TArray<class AObstacle*> obstacles;

	UPROPERTY(EditAnywhere, Category = "Obstacles")
		float obstacleExistanceChance; // A chance that determines whether an obstacle stays on the field or gets destroyed

	UPROPERTY(EditAnywhere, Category = "Emitters")
		TArray<UParticleSystem*> elementalEmitters;

	UPROPERTY(EditAnywhere, Category = "Battle")
		class ABattleManager* btlManager;

	int GetEmitterIndex(CurrentElemntalStat elementStat_);

public:	
	void AddObstacle(AObstacle* obstacle_, bool persistanceObstacle_);
	UParticleSystemComponent* SpawnElemntalEmitterAtLocation(FVector loc_, CurrentElemntalStat elementStat_);
	int GetBattlePhase();
	void TellObstaclesAPhaseHasPassed(int phase_);
	void RemoveObstacle(AObstacle* obstacle_);
	AObstacle* GetAnObstacleAtRandom();
	void ActivateOutlines(bool value_);

};
