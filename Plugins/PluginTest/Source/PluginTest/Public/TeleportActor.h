// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportActor.generated.h"

UCLASS()
class PLUGINTEST_API ATeleportActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleportActor();

	// variables
	UPROPERTY(EditAnywhere)
	float teleportTimer;

	FTimerHandle teleportHandle;

	UPROPERTY(EditAnywhere)
	int range;

	bool shouldTeleport;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Teleport();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
