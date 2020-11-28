// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleportActor.h"
#include "TimerManager.h"

// Sets default values
ATeleportActor::ATeleportActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATeleportActor::BeginPlay()
{
	Super::BeginPlay();
	shouldTeleport = true;
}

void ATeleportActor::Teleport()
{
	// REMINDER: use the root of the scene, don't use GetActor because it won't work
	if (USceneComponent* root = GetRootComponent())
	{
		// pick random location within the range
		FVector target = FVector(root->GetComponentLocation().X + FMath::RandRange(-1, 1) * FMath::RandRange(0, range),
								 root->GetComponentLocation().Y + FMath::RandRange(-1, 1) * FMath::RandRange(0, range),
								 root->GetComponentLocation().Z);

		// setLocation to that point

		root->SetWorldLocation(target);

		// after the teleport set this to true so teleport can occure again
		shouldTeleport = true;
		UE_LOG(LogTemp, Warning, TEXT("Teleport Ran"));
	}
	
}

// Called every frame
void ATeleportActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (shouldTeleport)
	{
		UE_LOG(LogTemp, Warning, TEXT("Teleport timer activated"));
		shouldTeleport = false;
		GetWorldTimerManager().SetTimer(teleportHandle, this, &ATeleportActor::Teleport, teleportTimer, false);
	}

}

