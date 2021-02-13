// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SRPGGameMode.h"
#include "SRPGPlayerController.h"
#include "SRPGCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/GameViewportClient.h"
#include "GenericPlatform/GenericPlatformProcess.h"

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

void ASRPGGameMode::SwitchLevel(FName levelName_)
{
	UGameplayStatics::OpenLevel(GetWorld(), levelName_);
}

void ASRPGGameMode::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle timerHandle;

	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ASRPGGameMode::DelegateWindowClosed, 0.5f, false);
}

void ASRPGGameMode::SwitchToNextLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), nextLevelName);
}

void ASRPGGameMode::SetNextLevelName(FName levelName_)
{
	nextLevelName = levelName_;
}

void ASRPGGameMode::DelegateWindowClosed()
{
	/*UGameInstance* instance_ = GetGameInstance();

	if (instance_)
	{
		UGameViewportClient* viewPortClient = instance_->GetGameViewportClient();
		if (viewPortClient)
		{
			viewPortClient->OnWindowCloseRequested().BindRaw(this, &ASRPGGameMode::UpdateAltf4);
		}
	}
	*/
}
bool ASRPGGameMode::UpdateAltf4()
{
	FGenericPlatformProcess::LaunchURL(TEXT("https://docs.google.com/forms/d/e/1FAIpQLSdFOjLXzURcEbgA3qiofX0pP2YVlmWzlatUXHohjM-SjcVJpA/viewform"), NULL,NULL);
	return true;
}
