// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "../ExternalFileReader/FighterTableStruct.h"
#include "../ExternalFileReader/EquipmentTableStruct.h"
#include "../ExternalFileReader/ItemTableStruct.h"
#include "Kismet/GameplayStatics.h"
#include "SRPGSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API USRPGSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	USRPGSaveGame();

	UPROPERTY(VisibleAnywhere, Category = Basic)
		FString saveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
		uint32 userIndex;

	UPROPERTY(VisibleAnywhere, Category = Fighters)
		TArray<FFighterTableStruct> currentRecruitedFightersState;
	UPROPERTY(VisibleAnywhere, Category = Equipment)
		TArray<FEquipmentTableStruct> currentWeaponsState;
	UPROPERTY(VisibleAnywhere, Category = Equipment)
		TArray<FEquipmentTableStruct> currentArmorsState;
	UPROPERTY(VisibleAnywhere, Category = Equipment)
		TArray<FEquipmentTableStruct> currentAccessoriesState;
	UPROPERTY(VisibleAnywhere, Category = Items)
		TArray<FItemTableStruct> currentItemsState;
	UPROPERTY(VisibleAnywhere, Category = Day)
		uint32 currentDayState;
	UPROPERTY(VisibleAnywhere, Category = Day)
		uint32 currentGoldState;
	UPROPERTY(VisibleAnywhere, Category = Day)
		uint32 currentShardsState;




	
	
};
