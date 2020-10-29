// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../SRPGGameMode.h"
#include "SRPGInvadeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API ASRPGInvadeGameMode : public ASRPGGameMode
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	ASRPGInvadeGameMode();
	void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

protected:
	int playerIndex; // Used to determine the type of pawn to spawn after the initial dummy pawn
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<class ABattlePawn>> battlePawns; //The pawns to be spawned


};
