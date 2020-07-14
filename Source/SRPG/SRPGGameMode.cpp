// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SRPGGameMode.h"
#include "SRPGPlayerController.h"
#include "SRPGCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASRPGGameMode::ASRPGGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ASRPGPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}