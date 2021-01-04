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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int skillIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool useItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bDeath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bStatDecrease;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bMiss;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHitOrCrit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int damage;

	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties") //Called on every tick
		void UpdateAnimationProperties();


public:

	void WeaponAttack();
	void GotHit(bool hitOrCrit_ );
	void SetDamage(int damage_);
	void GotMiss();
	void SkillAttack(int index_);
	void SetUseItem();
	void DeathAnim();
	void ChangeStats(bool stat_);
	
};
