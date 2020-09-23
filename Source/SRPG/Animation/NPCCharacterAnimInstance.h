// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NPCCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API UNPCCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	UNPCCharacterAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isWalking;

public:

	void SetIsWalking(bool walking_);
	
};
