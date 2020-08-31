// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FSkillTableStruct.generated.h"

USTRUCT(BlueprintType)
struct SRPG_API FSkillTableStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:


	FSkillTableStruct()
	{

	}

	UPROPERTY(BlueprintReadOnly)
		FString name;
	UPROPERTY(BlueprintReadWrite)
		FString description; //EditAnywhere, BluePrintReadOnly
	UPROPERTY(BlueprintReadOnly)
		int weaponIndex; //Determines which fighters can use this skill based on their weapon index.
	UPROPERTY(BlueprintReadOnly)
		int value; //The actual value for attacking or healing
	UPROPERTY(BlueprintReadOnly)
		int pip;//How many pips this skill expends
	UPROPERTY(BlueprintReadOnly)
		float atk;//A percentage. This determines how much the skills scales with attack. A 50% means that skill's value = value + atk*0.5f
	UPROPERTY(BlueprintReadOnly)
		float inti; //Scale with inti percentage
	UPROPERTY(BlueprintReadOnly)
		float crit;
	UPROPERTY(BlueprintReadOnly)
		float hit;
	UPROPERTY(BlueprintReadOnly)
		int crd;
	UPROPERTY(BlueprintReadOnly)
		int levelToUnlock; //The fighter's level must be >= than the skill level to unlock it.
	UPROPERTY(BlueprintReadOnly)
		int fls; //How much the skill affects the crowd meter
	UPROPERTY(BlueprintReadOnly)
		int rge; //Skill range
	UPROPERTY(BlueprintReadOnly)
		int rows; //Row speed -- Used for targeting
	UPROPERTY(BlueprintReadOnly)
		int depths; //Depth speed -- Used for targeting
	UPROPERTY(BlueprintReadOnly)
		int animationIndex; //Determines the animation that will play
	UPROPERTY(BlueprintReadOnly)
		int statusEffect;// 0 None 1 Poison 2 Paralysis  3  Terrified 4 Bleeding
	UPROPERTY(BlueprintReadOnly)
		int emitterIndex;
	UPROPERTY(BlueprintReadOnly)
		int statIndex; //Which stat this skill affects
	UPROPERTY(BlueprintReadOnly)
		int equipmentSkillsIndex;
	UPROPERTY(BlueprintReadOnly)
		int pure; //Pure vertical horizontal

};
