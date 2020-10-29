// Fill out your copyright notice in the Description page of Project Settings.


#include "SRPGInvadeGameMode.h"
#include "Battle/BattleController.h"
#include "SRPGGameState.h"

void ASRPGInvadeGameMode::BeginPlay()
{
	Super::BeginPlay();
}

ASRPGInvadeGameMode::ASRPGInvadeGameMode()
{
	GameStateClass = ASRPGGameState::StaticClass();
	playerIndex = 0;
}


void ASRPGInvadeGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	Super::FinishRestartPlayer(NewPlayer, StartRotation);


}

UClass* ASRPGInvadeGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (playerIndex == 0)
	{
		if (battlePawns.Num() > 0)
		{
			playerIndex++;
			return battlePawns[0];

		}

	}
	else if (playerIndex == 1)
	{
		if (battlePawns.Num() > 1)
			return battlePawns[1];
	}

	return DefaultPawnClass;
}