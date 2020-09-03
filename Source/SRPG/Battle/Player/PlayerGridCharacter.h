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

	APlayerGridCharacter();
	UFUNCTION(BlueprintCallable)
		void HighlightMovementPath();

	UFUNCTION(BlueprintCallable)
		void HighlightRegularAttackPath();

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void RemoveWidgetFromVP();

public:
	void Selected() override;
	void NotSelected() override;
	virtual void ActivateWeaponAttack() override;
	virtual void ActivateSkillAttack() override;
	void SetFighterIndex(int index_);

	
};
