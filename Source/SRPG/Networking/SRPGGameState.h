// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SRPGGameState.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API ASRPGGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ASRPGGameState();
	//// let's make some test variables and try replicating them to the client 
	UPROPERTY(ReplicatedUsing = OnRep_Test)
	int test = 5;
	UPROPERTY(Replicated)
	float test2 = 7.654f;

	UFUNCTION()
	void OnRep_Test();

	virtual void GetLifetimeReplicatedProps(class TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;
	
};
