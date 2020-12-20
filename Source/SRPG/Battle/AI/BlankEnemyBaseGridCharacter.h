// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/AI/EnemyBaseGridCharacter.h"
#include "BlankEnemyBaseGridCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API ABlankEnemyBaseGridCharacter : public AEnemyBaseGridCharacter
{
	GENERATED_BODY()
public:
	ABlankEnemyBaseGridCharacter();
	void SetManagers(AAIManager* ref_, AGridManager* gref_, ABattleManager* bref_, ABattleCrowd* cref_) override;
	void StartEnemyTurn() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Equipment")
		uint32 equippedWeapon;
	UPROPERTY(EditAnywhere, Category = "Equipment")
		uint32 equippedArmor;
	UPROPERTY(EditAnywhere, Category = "Equipment")
		uint32 equippedAccessory;

	void BeginPlay() override;
	void AddEquipmentStats(int tableIndex_) override;
	
};
