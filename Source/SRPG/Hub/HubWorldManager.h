// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hub/NPCs/BranchNPC.h"
#include "Hub/NPCs/CentralNPC.h"
#include "Hub/NPC.h"
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
	UPROPERTY(EditAnywhere)
	USceneComponent* root;
	// npc locations, manually chosen used to spawn NPC's
	UPROPERTY(EditAnywhere)
	TArray<AActor*> npcLocations;
	// randomly chosen
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ACentralNPC>> centralNPCs;
	// randomly chosen after the central NPCs were spawned
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ABranchNPC>> branchNPCs;
	// randomly chosen, 6 at max
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ANPC>> regularNPCs;
	//UPROPERTY(EditAnywhere)
	//TArray<TSubclassOf<ATournament>> tournamentNPCs;
	//UPROPERTY(EditAnywhere)
	//TArray<TSubclassOf<AItemShop>> itemShopNPCs;
	//UPROPERTY(EditAnywhere)
	//TArray<TSubclassOf<AFightersShop>> fighterShopNPCs;

	UPROPERTY(EditAnywhere)
	class ATournament* tournament;
	UPROPERTY(EditAnywhere)
	class AItemShop* itemShop;
	UPROPERTY(EditAnywhere)
	class AFightersShop* fighterShop;

	// initialises at level 2. Used by shops to determine what they can sell.
	int hubWorldLevel;
	// checked by NPC's to know which line to say
	bool nextIsStoryMission;
	int timeSlots;
	UPROPERTY(EditAnywhere)
	int maxTimeSlots;
	// ensures that a NPC cannot spawn in the same place
	TArray<bool> hasSpawned;
	// save an array of the npcs spawned
	TArray<class ANPC*> npcs;
	// is this the first time the manager is spawning npcs (also called after a reset)
	bool firstTimeSpawn;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetNextIsAStoryMission(bool value_);
	bool GetNextIsAStoryMission();
	int GetHubWorldLevel();
	int GetCurrentTimeSlotsCount();
	void UpdateTimeSlots(int value_);
	void UpdateJournal(bool battle_, FString line_);
	// how many NPC's to spawn and what type
	void SpawnNPCs(int num_, int type_);
	void SpawnDefaultNPCs(AActor* a_);
	void SpawnCentralNPCs(AActor* a_);
	void SpawnBranchNPCs(AActor* a_);
	//void SpawnTournamentNPC(AActor* a_);
	//void SpawnItemShop(AActor* a_);
	//void SpawnFighterShop(AActor* a_);
	// go through and remove the npcs
	void DeleteNPCs();
};
