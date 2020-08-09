// Fill out your copyright notice in the Description page of Project Settings.


#include "HubWorldManager.h"
#include "Hub/NPCs/BranchNPC.h"
#include "Hub/NPCs/Tournament.h"
#include "SRPGPlayerController.h"
#include "Engine/World.h"
#include "SRPGCharacter.h"

// Sets default values
AHubWorldManager::AHubWorldManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;
}

// Called when the game starts or when spawned
void AHubWorldManager::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld())
	{
		// sets the player ref to the first player controller in the world
		player = Cast<ASRPGCharacter>(GetWorld()->GetFirstPlayerController());
	}
	// sets the current time slots to our max time slots
	timeSlots = maxTimeSlots;
}

// Called every frame
void AHubWorldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHubWorldManager::SetNextIsAStoryMission(bool value_)
{
	nextIsStoryMission = value_;
}

bool AHubWorldManager::GetNextIsAStoryMission()
{
	return nextIsStoryMission;
}

int AHubWorldManager::GetHubWorldLevel()
{
	return hubWorldLevel;
}

int AHubWorldManager::GetCurrentTimeSlotsCount()
{
	return timeSlots;
}

void AHubWorldManager::UpdateTimeSlots(int value_)
{
	timeSlots -= value_;
	if (timeSlots < 0)
	{
		timeSlots = 0;
	}
}

void AHubWorldManager::UpdateJournal(bool battle_, FString line_)
{
	// journal functionality will be added in later
}

