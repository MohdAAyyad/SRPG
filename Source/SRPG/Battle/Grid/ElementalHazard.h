// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/Grid/Obstacle.h"
#include "ElementalHazard.generated.h"

UENUM()
enum class CurrentElemntalStat : uint8
{
	NONE = 0,
	OIL = 1,
	WATER = 2,
	POISON = 3,
	FIRE = 4,
	FROZEN = 5,
	PARALYSIS = 6,
	EXPLOSION = 7,
	STEAM = 8
};
UCLASS()
class SRPG_API AElementalHazard : public AObstacle
{
	GENERATED_BODY()
public:

	AElementalHazard();

	void ObstacleTakeDamage(float damage_, int statusEffect_) override;
	bool AddObstructedTile(ATile* tile_) override;
	void DelayedDestroy() override;

	void ATurnHasPassed(int turnType_) override;

	CurrentElemntalStat GetCurrentState();

protected:

	int turnsSinceStateChanged;
	int turnThatStatChangeOccuredIn;
	bool bDying;
	bool bShrinkingMesh;
	float delayUntilDead;
	class UParticleSystemComponent* currentParticles;
	FVector particleLocation;

	UPROPERTY(EditAnywhere, Category = "Stats")
		float explosionDamage;

	UPROPERTY(EditAnywhere, Category = "DecalActor")
		TSubclassOf<class ADecalActor> decal;
	
	UPROPERTY(EditAnywhere, Category = "DecalActor")
		UMaterialInterface* decalMaterial;

	ADecalActor* decalact;

	void BeginPlay() override;
	void Tick(float DeltaTime) override;

	void AddObstacleToObstacleManager() override;

	void ActivateOutline(bool value_) override;

	UFUNCTION()
		void OverlapWithGridCharacter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
		CurrentElemntalStat currentStat;

	
};
