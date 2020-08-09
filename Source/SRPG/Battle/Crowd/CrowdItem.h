// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrowdItem.generated.h"

UCLASS()
class SRPG_API ACrowdItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrowdItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Root")
		USceneComponent* root;
	UPROPERTY(EditAnywhere, Category = "Collision")
		class USphereComponent* sphere;
	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystem* spawnParticles;
	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystemComponent* particles;
	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystem* endParticles;
	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystem* obtainedParticles;

	int turnsElapsedSinceSpawned; 
	UPROPERTY(EditAnywhere, Category = "Item")
		int turnsUntilItemIsDestroyed;
	UPROPERTY(EditAnywhere, Category = "Item")
		int statIndex;
	UPROPERTY(EditAnywhere, Category = "Item")
		int value;
	class ABattleManager* btlManager;
	class ABattleCrowd* crdManager;
	class AEnemyBaseGridCharacter* markedByThisEnemy;

	bool bMarkedByEnemy; //Used to make sure two enemies don't go after the same item

	UFUNCTION()
		void OverlapWithGridCharacter(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult &sweepResult_);

	void DespawnItem();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetBtlAndCrdManagers(ABattleManager* btl_, ABattleCrowd* crd_);
	void CheckIfItemShouldBeDestroyed();
	bool MarkItem(AEnemyBaseGridCharacter* markingEnemy_); //Called by enemies to make sure no one else takes this item. Return true if operation was a success
	int GetStat();
	int GetValue();
	class ATile* GetMyTile();
	void Obtained(FVector loc_);
	void ItemWasObtainedByAnEnemyThatDidNotMarkIt();

};
