// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "NPCWanderComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SRPG_API UNPCWanderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNPCWanderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// this is where the NPC wants to go
	FVector target;
	// set this when we spawn
	FVector originalPos;
	ACharacter* chara;
	// what's the maximum degree of turning
	// how fast can the NPC move
	UPROPERTY(EditAnywhere)
	float maxSpeed;
	UPROPERTY(EditAnywhere)
	float maxWander;
	UPROPERTY(EditAnywhere)
	float waitTime;
	// time which will wait as the npc wanders to find a new target so it doesn't get stuck
	UPROPERTY(EditAnywhere)
	float findNewTargetTime;
	UPROPERTY(EditAnywhere)
	int maxDistance;
	UPROPERTY(EditAnywhere)
	float radius;
	FTimerHandle newTargetTimerHandle;
	FTimerHandle findNewTargetTimerHandle;
	UPROPERTY(EditAnywhere)
	float timeToArrive;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION()
	void Wander();
	UFUNCTION()
	void SelectNewTargetLocation();

	bool shouldMove;

	bool GetShouldMove();

	void SetCharacterRef(ACharacter* chara_);

		
};
