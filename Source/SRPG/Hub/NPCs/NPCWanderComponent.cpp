// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCWanderComponent.h"

#include "Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UNPCWanderComponent::UNPCWanderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}


// Called when the game starts
void UNPCWanderComponent::BeginPlay()
{
	Super::BeginPlay();
	shouldMove = false;
	//SelectNewTargetLocation();
	// ...
	
}


// Called every frame
void UNPCWanderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UNPCWanderComponent::Wander()
{

	if (shouldMove)
	{
		GetWorld()->GetTimerManager().SetTimer(findNewTargetTimerHandle, this, &UNPCWanderComponent::SelectNewTargetLocation, findNewTargetTime, true);
		FVector direction;
		// this function is going to calculate how the npc needs to move
		direction = target - chara->GetActorLocation();
		float dist = FVector::Dist(target, chara->GetActorLocation());
		//UE_LOG(LogTemp, Warning, TEXT("Scooting"));
		// if we are in the radius go home 
		if (dist < radius)
		{
			shouldMove = false;
			GetWorld()->GetTimerManager().SetTimer(newTargetTimerHandle, this, &UNPCWanderComponent::SelectNewTargetLocation, waitTime, true);
			GetWorld()->GetTimerManager().ClearTimer(findNewTargetTimerHandle);
			//UE_LOG(LogTemp, Warning, TEXT("Arrived"));
			return;
		}

		direction *= maxSpeed / timeToArrive;

		// if we are far enough away just start booking it
		if (direction.Size() > maxSpeed)
		{
			direction.Normalize();
			direction *= maxSpeed;
		}

		// interpolate between the 
		/*FVector interp = FMath::Lerp(chara->GetActorRotation().Vector(), direction, 5);
		FRotator rot = FRotator(0, 0, interp.Z);*/
		// finally take the movement input and add that in
		chara->AddMovementInput(direction);
	}
}

void UNPCWanderComponent::SelectNewTargetLocation()
{
	if (shouldMove == false)
	{
		// spawn a new target 
		FVector tempTarget = FVector(originalPos.X + FMath::RandRange(-1, 1) * FMath::RandRange(0, maxDistance),
									 originalPos.Y + FMath::RandRange(-1, 1) * FMath::RandRange(0, maxDistance),
									 originalPos.Z);

		if (tempTarget.X - originalPos.X <= maxDistance || tempTarget.Y - originalPos.Y <= maxDistance)
		{
			
			target = tempTarget;
			shouldMove = true;
			//UE_LOG(LogTemp, Warning, TEXT(" Target X: %f Y: %f Z: %F"), target.X, target.Y, target.Z);
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("Target Location is outside of the maximum distance"));
			SelectNewTargetLocation();
			shouldMove = false;
		}
	}
}

bool UNPCWanderComponent::GetShouldMove()
{
	return shouldMove;
}

void UNPCWanderComponent::SetCharacterRef(ACharacter* chara_)
{
	chara = chara_;
	originalPos = chara->GetActorLocation();
	SelectNewTargetLocation();
}


