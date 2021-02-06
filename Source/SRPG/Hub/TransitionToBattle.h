// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "../ExternalFileReader/FighterTableStruct.h"
#include "../ExternalFileReader/FDayTableStruct.h"
#include "TransitionToBattle.generated.h"

UCLASS()
class SRPG_API ATransitionToBattle : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATransitionToBattle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class ASRPGGameMode* gameMode;

	bool bHasBeenInteractedWith;
	UPROPERTY(BlueprintReadWrite)
	bool bOnlinePlay;

	UPROPERTY(EditAnywhere, Category = "Root")
		USceneComponent* root;
	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystemComponent* particles;
	UPROPERTY(EditAnywhere, Category = "Collisions")
		class UBoxComponent* box;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Tourney") //Will be used when displaying information about the next op when the player is selecting the fighters
		class ATournament* tourney;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widget")
		class UWidgetComponent* widgetComp;
	UPROPERTY(EditAnywhere, Category = "FileReader")
		class UExternalFileReader* fileReader;
	UPROPERTY(EditAnywhere, Category = "Audio Manager")
		class AAudioMnager* audioMgr;

	TArray<FFighterTableStruct> recruitedFighters;

	UPROPERTY(BlueprintReadOnly)
		FDayTableStruct dayInfo;

	UPROPERTY(BlueprintReadOnly)
		TArray<int> indexesOfSelectedFighters; //Saves the indexes of fighters selected for the battle from the recruited fighters array

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<UMaterialInterface*> fighterMaterials; //Ordered the same way as the BPs array

	UFUNCTION()
		void OverlapWithPlayer(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult &sweepResult_);

	UFUNCTION(BlueprintCallable) //Called from UI
		void EndDay();

	UFUNCTION(BlueprintCallable)
		TArray<FFighterTableStruct> GetRecruitedFighters();
	UFUNCTION(BlueprintCallable)
		void RemoveRecruitedFighterAtIndex(int index_); //Used for removing selected fighters
	UFUNCTION(BlueprintCallable)
		void AddFighterToSelectedFighters(int index_);
	UFUNCTION(BlueprintCallable)
		void FinalizeFighterSelection(); //Passes in the indexes to the intermediate

	UFUNCTION(BlueprintCallable)
		void InitiateAReRun();


	UFUNCTION(BlueprintCallable)
		void UpdateNextLevelName();

	void UpdateRecruitedFighters();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Interact() override;
	void UnInteract() override;

};
