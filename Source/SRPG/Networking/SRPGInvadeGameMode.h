// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SRPGInvadeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API ASRPGInvadeGameMode : public AGameMode
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	ASRPGInvadeGameMode();
};
