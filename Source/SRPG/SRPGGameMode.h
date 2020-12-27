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

public:
	ASRPGGameMode();
	UFUNCTION(BlueprintCallable)
		void SwitchLevel(FName levelName);


	virtual void BeginPlay() override;
};



