// Fill out your copyright notice in the Description page of Project Settings.


#include "HubWorldManager.h"
#include "Hub/NPCs/BranchNPC.h"
#include "Hub/NPCs/Tournament.h"
#include "Hub/NPCs/ItemShop.h"
#include "Hub/NPCs/FightersShop.h"
#include "SRPGPlayerController.h"
#include "Engine/World.h"
#include "SRPGCharacter.h"
#include "Intermediary/Intermediate.h"

// Sets default values
AHubWorldManager::AHubWorldManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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
	hasSpawned.Init(false, NPCLocations.Num());


	SpawnNPCs(6, 0);
	DeleteNPCs();
	SpawnNPCs(1, 0);
	SpawnNPCs(1, 1);
	SpawnNPCs(1, 2);
	SpawnNPCs(1, 3);
	SpawnNPCs(1, 4);
	SpawnNPCs(1, 5);

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
		// tell the intermediate that we're out of timeslots

		timeSlots = 0;
	}
}

void AHubWorldManager::UpdateJournal(bool battle_, FString line_)
{
	// journal functionality will be added in later
}

// 0 = npc, 1 = central, 2 = branch, 3 = tournament, 4 = item shop, 5 = fighter shop
void AHubWorldManager::SpawnNPCs(int num_, int type_)
{
	// spawn NPC's based on the actor locations
	// check to see if world is null
	int spawned = 0;
	//TArray<bool> hasSpawned;
	// this is the maximum amount of times the while loop will be allowed to run
	// takes the possible locations and multiplies it by the number of npcs it should spawn, adds 5 for good mesure
	int maxLoopCount = NPCLocations.Num() * num_ + 5;
	int totalLoopCount = 0;
	while (spawned < num_)
	{
		
		int spawnChance = FMath::RandRange(0, NPCLocations.Num());
		int j = 0;

		if (GetWorld())
		{
			// i'll make this more intelligent with selecting certain places to spawn it
			// make an array to tell what locations we've already spawned
			for (auto a : NPCLocations)
			{
				// in the future we would do some RNG to determine what type of NPC is spawned but for now we'll keep it simple
				if (j == spawnChance && hasSpawned[j] == false && spawned < num_)
				{
					switch (type_)
					{
					 case 0 :
						 SpawnDefaultNPCs(a);
						 hasSpawned[j] = true;
						 spawned++;
						 break;
					 case 1:
						 SpawnCentralNPCs(a);
						 hasSpawned[j] = true;
						 spawned++;
						 break;
					 case 2:
						 SpawnBranchNPCs(a);
						 hasSpawned[j] = true;
						 spawned++;
						 break;
					 case 3:
						 SpawnTournamentNPC(a);
						 hasSpawned[j] = true;
						 spawned++;
						 break;
					 case 4:
						 SpawnItemShop(a);
						 hasSpawned[j] = true;
						 spawned++;
						 break;
					 case 5:
						 SpawnFighterShop(a);
						 hasSpawned[j] = true;
						 spawned++;
						 break;
					}
					
				}

				j++;
			}
		}
		totalLoopCount++;
		// if we aren't able to find a valid location in a certain amount of loops 
		if (totalLoopCount >= maxLoopCount)
		{
			// go home 
			UE_LOG(LogTemp, Error, TEXT("Not able to find valid location for all NPCs. %d NPCs Failed to spawn. Make sure you have enough spawn points avaible."), num_ - spawned);
			break;
		}
	}
	
}

void AHubWorldManager::SpawnDefaultNPCs(AActor* a_)
{
	//spawnChance = FMath::RandRange(j + 1, NPCLocations.Num());
	ANPC* npc = GetWorld()->SpawnActor<ANPC>(regularNPCs[0]->GetClass(), a_->GetActorTransform());
	if (npc)
	{
		npc->SetNPCLinesIndex(2);
		npc->SetHubManager(this);
		npcs.Push(npc);
	}
	UE_LOG(LogTemp, Warning, TEXT("Spawned Default NPC"));
}

void AHubWorldManager::SpawnCentralNPCs(AActor* a_)
{
	ACentralNPC* centralNpc = GetWorld()->SpawnActor<ACentralNPC>(centralNPCs[0]->GetClass(), a_->GetActorTransform());
	if (centralNpc)
	{
		centralNpc->SetHubManager(this);
		npcs.Push(centralNpc);
	}
	// any information we need to give the NPC will be done here
	UE_LOG(LogTemp, Warning, TEXT("Spawned Central NPC"));
}

void AHubWorldManager::SpawnBranchNPCs(AActor* a_)
{
	ABranchNPC* branchNpc = GetWorld()->SpawnActor<ABranchNPC>(branchNPCs[0]->GetClass(), a_->GetActorTransform());
	if (branchNpc)
	{
		branchNpc->SetHubManager(this);
		npcs.Push(branchNpc);
	}
	UE_LOG(LogTemp, Warning, TEXT("Spawned Branch NPC"));
}

void AHubWorldManager::SpawnTournamentNPC(AActor* a_)
{
	ATournament* tournamentNpc = GetWorld()->SpawnActor<ATournament>(tournamentNPCs[0]->GetClass(), a_->GetActorTransform());
	tournament = tournamentNpc;
	if (tournamentNpc)
	{
		tournamentNpc->SetHubManager(this);
		npcs.Push(tournamentNpc);
	}
	UE_LOG(LogTemp, Warning, TEXT("Spawned Tournament NPC"));
}

void AHubWorldManager::SpawnItemShop(AActor* a_)
{
	AItemShop* itemShop = GetWorld()->SpawnActor<AItemShop>(itemShopNPCs[0]->GetClass(), a_->GetActorTransform());
	if (itemShop)
	{
		itemShop->SetHubManager(this);
		npcs.Push(itemShop);
	}
	UE_LOG(LogTemp, Warning, TEXT("Spawned ItemShop NPC"));
}

void AHubWorldManager::SpawnFighterShop(AActor* a_)
{
	AFightersShop* fighterShop = GetWorld()->SpawnActor<AFightersShop>(fighterShopNPCs[0]->GetClass(), a_->GetActorTransform());
	if (fighterShop)
	{
		fighterShop->SetHubManager(this);
		npcs.Push(fighterShop);
	}
	UE_LOG(LogTemp, Warning, TEXT("Spawned FighterShop NPC"));
}

void AHubWorldManager::DeleteNPCs()
{
	// npc deletion loop
	for (auto a : npcs)
	{
		// run through and delete all of the added npcs
		a->Destroy();
		
		UE_LOG(LogTemp, Warning, TEXT("Deleted NPC"));
	}

	for (int i = npcs.Num() - 1; i >= 0; i--)
	{
		// remove the npc ref from the array
		npcs.RemoveAt(i, 1, true);
	}
	// reset the array
	hasSpawned.Init(false, NPCLocations.Num());

}
