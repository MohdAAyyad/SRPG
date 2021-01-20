// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileGridCharacter.generated.h"

UCLASS()
class SRPG_API AProjectileGridCharacter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileGridCharacter();

protected:
	UPROPERTY(EditAnywhere)
		USceneComponent* root;

	UPROPERTY(EditAnywhere, Category = "Movement")
		class UProjectileMovementComponent* movement;

	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystemComponent* particles;

public:	

};
