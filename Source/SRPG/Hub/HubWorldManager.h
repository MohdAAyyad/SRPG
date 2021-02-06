// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hub/NPCs/BranchNPC.h"
#include "Hub/NPCs/CentralNPC.h"
#include "Hub/NPC.h"
#include "Networking/SRPGGameState.h"
#include "ExternalFileReader/FOpponentStruct.h"
#include "HubWorldManager.generated.h"

USTRUCT(BlueprintType)

struct SRPG_API FAllFighterInfoStruct
{
	GENERATED_USTRUCT_BODY()
public:
	FAllFighterInfoStruct()
	{

	}
	UPROPERTY(BlueprintReadOnly)
		FFighterTableStruct fighter;
	UPROPERTY(BlueprintReadOnly)
		TArray<FSkillTableStruct> weaponSkills;
	UPROPERTY(BlueprintReadOnly)
		TArray<FSkillTableStruct> armorSkills;
	UPROPERTY(BlueprintReadOnly)
		FEquipmentTableStruct weapon;
	UPROPERTY(BlueprintReadOnly)
		FEquipmentTableStruct armor;
	UPROPERTY(BlueprintReadOnly)
		FEquipmentTableStruct accessory;
};

UCLASS()
class SRPG_API AHubWorldManager : public AActor
{
	GENERATED_BODY()
	
public:

	//Read by the pause menu
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<UTexture*> itemTextures;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<UTexture*> weaponTextures;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<UTexture*> armorTextures;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UTexture* accessoryTexture;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<UMaterialInterface*> fighterTextures;


	UPROPERTY(EditAnywhere, Category = "File Reader")
		class UExternalFileReader* fileReader;

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
	UPROPERTY(EditAnywhere)
	TArray<class ACutscene*> cutscenes;
	UPROPERTY(EditAnywhere)
	TArray<USkeletalMesh*> meshes;

	UPROPERTY(EditAnywhere)
	class ATournament* tournament;
	UPROPERTY(EditAnywhere)
	class AItemShop* itemShop;
	UPROPERTY(EditAnywhere)
	class AFightersShop* fighterShop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* pauseMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetComponent* generalWidget;

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

	bool firstTimeInfoSpawn;


	void GetPlayerFromController();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetNextIsAStoryMission(bool value_);
	bool GetNextIsAStoryMission();
	int GetHubWorldLevel();
	UFUNCTION(BlueprintCallable)
		int GetCurrentTimeSlotsCount();
	UFUNCTION(BlueprintCallable)
		int GetCurrentMoney();
	UFUNCTION(BlueprintCallable)
		int GetCurrentShards();
	UFUNCTION(BlueprintCallable)
		int GetCurrentDay();

	UFUNCTION(BlueprintCallable)
		bool CheckForRewards();
	void UpdateTimeSlots(int value_);
	void UpdateJournal(bool battle_, FString line_);
	// how many NPC's to spawn and what type
	void SpawnNPCs(int num_, int type_);
	// spawn a single NPC of a given type and get a reference to it 
	ANPC* SpawnNPC(int type_);
	ACentralNPC* SpawnCentralNPC();
	ABranchNPC* SpawnBranchNPC();
	// used specifically for spawning central NPC's with a branch attached
	void SpawnNPCs(int num_, bool spawnBranch_);
	ANPC* SpawnDefaultNPCs(AActor* a_);
	ACentralNPC* SpawnCentralNPCs(AActor* a_);
	ABranchNPC* SpawnBranchNPCs(AActor* a_);
	//void SpawnTournamentNPC(AActor* a_);
	//void SpawnItemShop(AActor* a_);
	//void SpawnFighterShop(AActor* a_);
	// go through and remove the npcs
	void DeleteNPCs();
	void SpawnCentralNPCs(int amount_);

	void PlayCutscene(int index_);


	void SpawnInfoNPC(int archetype_, FOpponentStruct opp_);

	UFUNCTION(BlueprintCallable)
		void TogglePauseMenu();

	UFUNCTION(BlueprintCallable)
		TArray<FFighterTableStruct> GetAllRecruitedFighters();

	UFUNCTION(BlueprintCallable)
		FAllFighterInfoStruct GetFighterInfoByID(int id_);

	UFUNCTION(BlueprintCallable)
		TArray<FEquipmentTableStruct> GetEquipmentOfACertainType(int equipIndex, int subIndex_);

	UFUNCTION(BlueprintCallable)
		TArray<FSkillTableStruct>FindSkillsByAPieceOfEquipment(int equipIndex_, int subIndex_, int skillNum_, int skillsIndex_, int currentLevel_);

	UFUNCTION(BlueprintCallable)
		void Equip(int fighterID, int equipIndex, int equipID, int oldEquipID);


	UFUNCTION(BlueprintCallable)
		TArray<FItemTableStruct> GetAllOwnedItems();


};
