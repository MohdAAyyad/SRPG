// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/GridCharacter.h"
#include "ExternalFileReader/ItemTableStruct.h"
#include "PlayerGridCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API APlayerGridCharacter : public AGridCharacter
{
	GENERATED_BODY()

protected:
	int fighterID; //The ID is used to distinguish the fighter inside the recruited table

	UPROPERTY(BlueprintReadOnly, Category = "Skills")
		TArray<FSkillTableStruct> skills;
	int chosenSkillIndex;

	APlayerGridCharacter();
	UFUNCTION(BlueprintCallable)
		void HighlightMovementPath();

	UFUNCTION(BlueprintCallable)
		void HighlightRegularAttackPath();

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void RemoveWidgetFromVP();
	void FinishState() override;

	UFUNCTION(BlueprintCallable)
		void UseSkill(int index_);

	void UpdateCharacterSkills();

public:
	void Selected() override;
	void NotSelected() override;
	virtual void ActivateWeaponAttack() override;
	virtual void ActivateSkillAttack() override;
	void SetFighterIndex(int index_);
	void SetFighterID(int fighterID_);
	void StartPlayerTurn();
	void EndPlayerTurn();

	void GridCharTakeDamage(float damage_, AGridCharacter* attacker_) override;
	void UpdateCurrentEXP();
	void CheckChangeStats();


	
};
