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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		float forwardSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		bool bWeaponAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		bool bHit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		int skillIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		bool useItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		bool bDeath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		bool bStatDecrease;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		bool bMiss;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		bool bHitOrCrit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		int damage;

	UFUNCTION(Server, unreliable, BlueprintCallable, Category = "UpdateAnimationProperties") //Called on every tick, no need for it to be reliable
		void UpdateAnimationProperties();
		void UpdateAnimationProperties_Implementation();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

public:

	UFUNCTION(Client, Reliable)
	void WeaponAttack();
	void WeaponAttack_Implementation();
	UFUNCTION(Client, Reliable)
	void GotHit(bool hitOrCrit_ );
	void GotHit_Implementation(bool hitOrCrit_);
	UFUNCTION(Client, Reliable)
	void SetDamage(int damage_);
	void SetDamage_Implementation(int damage_);
	UFUNCTION(Client, Reliable)
	void GotMiss();
	void GotMiss_Implementation();
	UFUNCTION(Client, Reliable)
	void SkillAttack(int index_);
	void SkillAttack_Implementation(int index_);
	UFUNCTION(Client, Reliable)
	void SetUseItem();
	void SetUseItem_Implementation();
	UFUNCTION(Client, Reliable)
	void DeathAnim();
	void DeathAnim_Implementation();
	UFUNCTION(Client, Reliable)
	void ChangeStats(bool stat_);
	void ChangeStats_Implementation(bool stat_);
	
};
