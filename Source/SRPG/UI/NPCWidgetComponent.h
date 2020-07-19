// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Hub/NPC.h"
#include "NPCWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API UNPCWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
protected:

		
public:
	UFUNCTION(BlueprintCallable)
		void SetNPCRef(ANPC* npc_);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString line;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ANPC* npcRef;

};
