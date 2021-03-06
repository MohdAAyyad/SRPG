// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SRPGGameMode.generated.h"

UCLASS(minimalapi)
class ASRPGGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
		class AHubWorldManager* hubWorldManager;

	FName nextLevelName;

	void DelegateWindowClosed();
	bool UpdateAltf4();

public:
	ASRPGGameMode();
	UFUNCTION(BlueprintCallable)
		void SwitchLevel(FName levelName_);

	UFUNCTION(BlueprintCallable)
		void SwitchToNextLevel();

	UFUNCTION(BlueprintCallable)
		void TellIntermediateThisIsANewGame(); //Called from the UI when the player chooses start game instead of continue

	void SetNextLevelName(FName levelName_);


	virtual void BeginPlay() override;
};



