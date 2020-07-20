// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GridCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API UGridCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UGridCharacterAnimInstance();

	UPROPERTY(BlueprintReadOnly)
		class AGridCharacter* ownerGridCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float forwardSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bWeaponAttack;

	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
		void UpdateAnimationProperties();

public:

	void WeaponAttack();

	
};
