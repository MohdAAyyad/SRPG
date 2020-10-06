// Fill out your copyright notice in the Description page of Project Settings.


#include "SRPGInvadeGameMode.h"
#include "SRPGGameState.h"

void ASRPGInvadeGameMode::BeginPlay()
{
	Super::BeginPlay();
}

ASRPGInvadeGameMode::ASRPGInvadeGameMode()
{
	GameStateClass = ASRPGGameState::StaticClass();
}
