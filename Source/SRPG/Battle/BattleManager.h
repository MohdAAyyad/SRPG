// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	
	int numberOfUnitsDeployed;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
		class UWidgetComponent* widgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle") //Will be read by UI to show the phases
		int phase; //0 deployment 1 player 2 enemy 3 crowd 4 end
	
	//PLACEHOLDER. WON'T BE NEEDED ONCE TABLES AND INTERMEDIATE ARE UP AND RUNNING
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle") 
		TArray<int> selectedFighters;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
		TArray<int> deployedFightersIndexes; //Used to make sure the same unit is not spawned more than once
	TArray<class APlayerGridCharacter*> deployedUnits;

	UPROPERTY(EditAnywhere, Category = "AI")
		class AAIManager* aiManager;

	UPROPERTY(EditAnywhere, Category = "Crowd")
		class ABattleCrowd* crdManager;

	UFUNCTION(BlueprintCallable)
		void DeployThisUnitNext(int bpid_); //Called from the UI

	UPROPERTY(BlueprintReadOnly)
		int totalNumberOfPhasesElapsed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DeplyUnitAtThisLocation(FVector tileLoc_); //Called from controller
	int GetPhase();
	UFUNCTION(BlueprintCallable)
		void NextPhase();
	UFUNCTION(BlueprintCallable)
		void EndDeployment(); //Called from the UI 

	int GetTotalNumberOfPhasesElapsed();

	float GetTotalStatFromDeployedPlayers(int statIndex_);
	AGridCharacter* GetPlayerWithHighestStat(int statIndex_);
	AGridCharacter* GetPlayerWithLowestStat(int statIndex_);
	TArray<APlayerGridCharacter*> GetDeployedPlayers();

};
