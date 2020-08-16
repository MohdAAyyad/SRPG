// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
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

	UPROPERTY(EditAnywhere)
		FName nextBattleLevel;

	UFUNCTION()
		void OverlapWithPlayer(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult &sweepResult_);

	UFUNCTION(BlueprintCallable) //Called from UI
		void EndDay();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Interact() override;
	void UnInteract() override;

};
