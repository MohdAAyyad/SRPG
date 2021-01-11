// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowdProjectile.generated.h"

UCLASS()
class SRPG_API ACrowdProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ACrowdProjectile();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
		USceneComponent* root;
	UPROPERTY(EditAnywhere)
		class USphereComponent* sphere;
	UPROPERTY(EditAnywhere)
		class UParticleSystemComponent* particles;
	UPROPERTY(EditAnywhere)
		class UParticleSystem* emitter;
	UPROPERTY(EditAnywhere, Category = "Movement")
		class UProjectileMovementComponent* movement;
	UPROPERTY(EditAnywhere)
		int statusEffect;
	UFUNCTION()
		void OverlapWithObstacle(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

public:	

};
