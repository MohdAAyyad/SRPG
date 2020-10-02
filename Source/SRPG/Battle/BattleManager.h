// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExternalFileReader/FighterTableStruct.h"
#include "BattleManager.generated.h"

UCLASS()
class SRPG_API ABattleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABattleManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Root")
		 USceneComponent* root;

	UPROPERTY(EditAnywhere, Category = "Fighters")
		TArray<TSubclassOf<class AGridCharacter>> fighters;

	UPROPERTY(EditAnywhere, Category = "Deployment")
		class AGridManager* gridManager;
	UPROPERTY(EditAnywhere, Category = "Deployment")
		int rowIndexOfStartingTile;
	UPROPERTY(EditAnywhere, Category = "Deployment")
		int offsetOfStartingTile;
	UPROPERTY(EditAnywhere, Category = "Deployment")
		int deploymentRowSpeed;
	UPROPERTY(EditAnywhere, Category = "Deployment")
		int deploymentDepth;
	UPROPERTY(EditAnywhere, Category = "Deployment")
		int bpidOfUnitToBeDeployedNext;
	UPROPERTY(EditAnywhere, Category = "Deployment")
		int maxNumberOfUnitsToDeploy;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Deployment")
		TArray<UMaterialInterface*> fighterMaterials; //Ordered in the same way as the fighter BPs. Used by UI
	
	int numberOfUnitsDeployed;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
		class UWidgetComponent* widgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
		TArray<TSubclassOf<UUserWidget>> EndWidgets;

	bool bBattleHasEnded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle") //Will be read by UI to show the phases
		int phase; //0 deployment 1 player 2 enemy 3 crowd 4 end
	
	//Updated from the intermediate
	UPROPERTY(BlueprintReadOnly, Category = "Battle") 
		TArray<FFighterTableStruct> selectedFighters;

	int indexOfSelectedFighterInSelectedFighters;

	UPROPERTY(BlueprintReadOnly, Category = "Battle")
		TArray<int> deployedFightersIndexes; //Used to make sure the same unit is not spawned more than once
	UPROPERTY(BlueprintReadOnly, Category = "Deployed units")
		TArray<class APlayerGridCharacter*> deployedUnits;

	UPROPERTY(EditAnywhere, Category = "AI")
		class AAIManager* aiManager;

	UPROPERTY(EditAnywhere, Category = "Crowd")
		class ABattleCrowd* crdManager;

	UPROPERTY(EditAnywhere, Category = "Emitters")
		TArray<UParticleSystem*> skillEmitters;

	UPROPERTY(EditAnywhere, Category = "Emitters")
		TArray<UParticleSystem*> weaponEmitters;

	UFUNCTION(BlueprintCallable)
		void DeployThisUnitNext(int bpid_); //Called from the UI

	UPROPERTY(BlueprintReadOnly)
		int totalNumberOfPhasesElapsed;

	class ABattleController* btlCtrl;

	void UpdatePlayerEXP();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DeplyUnitAtThisLocation(FVector tileLoc_); //Called from controller
	int GetPhase();
	UFUNCTION(BlueprintCallable)
		void NextPhase();
	UFUNCTION(BlueprintCallable)
		void EndDeployment(); //Called from the UI 

	int GetBpidOfUnitToBeDeployedNext();
	int GetTotalNumberOfPhasesElapsed();

	float GetTotalStatFromDeployedPlayers(int statIndex_);
	AGridCharacter* GetPlayerWithHighestStat(int statIndex_);
	AGridCharacter* GetPlayerWithLowestStat(int statIndex_);
	TArray<APlayerGridCharacter*> GetDeployedPlayers();

	void EndBattle(bool victory_);
	void HandlePlayerDeath(APlayerGridCharacter* player_);

	void SpawnSkillEmitter(FVector loc_, int emitterIndex_);
	void SpawnWeaponEmitter(FVector loc_, int emitterIndex_);

};
