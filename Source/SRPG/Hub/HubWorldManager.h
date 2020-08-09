// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HubWorldManager.generated.h"

UCLASS()
class SRPG_API AHubWorldManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHubWorldManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// player ref
	class ASRPGCharacter* player;
	USceneComponent* root;
	// npc locations, manually chosen used to spawn NPC's
	TArray<AActor*> NPCLocations;
	// randomly chosen
	UPROPERTY(EditAnywhere)
	TArray<class ACentralNPC*> centralNPCs;
	// randomly chosen after the central NPCs were spawned
	UPROPERTY(EditAnywhere)
	TArray<class ABranchNPC*> branchNPCs;
	// randomly chosen, 6 at max
	UPROPERTY(EditAnywhere)
	TArray<class ANPC*> regularNPCs;

	UPROPERTY(EditAnywhere)
	class ATournament* tournament;

	// initialises at level 2. Used by shops to determine what they can sell.
	int hubWorldLevel;
	// checked by NPC's to know which line to say
	bool nextIsStoryMission;
	int timeSlots;
	UPROPERTY(EditAnywhere)
	int maxTimeSlots;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetNextIsAStoryMission(bool value_);
	bool GetNextIsAStoryMission();
	int GetHubWorldLevel();
	int GetCurrentTimeSlotsCount();
	void UpdateTimeSlots(int value_);
	void UpdateJournal(bool battle_, FString line_);

};
