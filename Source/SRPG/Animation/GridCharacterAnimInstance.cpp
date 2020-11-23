// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCharacterAnimInstance.h"
#include "../Battle/GridCharacter.h"
#include "UnrealNetwork.h"


UGridCharacterAnimInstance::UGridCharacterAnimInstance()
{
	ownerGridCharacter = nullptr;
	forwardSpeed = 0.0f;
	bWeaponAttack = false;
	bHit = false;
	skillIndex = -1;
	bDeath = false;
	bStatDecrease = false;
	bHitOrCrit = false;
	bMiss = false;
}
void UGridCharacterAnimInstance::UpdateAnimationProperties_Implementation()
{
	if (!ownerGridCharacter)
	{
		ownerGridCharacter = Cast<AGridCharacter>(TryGetPawnOwner());
	}
	else
	{
		forwardSpeed = ownerGridCharacter->GetVelocity().Size();
	}
}

void UGridCharacterAnimInstance::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	DOREPLIFETIME(UGridCharacterAnimInstance, forwardSpeed);
	DOREPLIFETIME(UGridCharacterAnimInstance, bWeaponAttack);
	DOREPLIFETIME(UGridCharacterAnimInstance, bHit);
	DOREPLIFETIME(UGridCharacterAnimInstance, skillIndex);
	DOREPLIFETIME(UGridCharacterAnimInstance, useItem);
	DOREPLIFETIME(UGridCharacterAnimInstance, bDeath);
	DOREPLIFETIME(UGridCharacterAnimInstance, bStatDecrease);
	DOREPLIFETIME(UGridCharacterAnimInstance, bMiss);
	DOREPLIFETIME(UGridCharacterAnimInstance, bHitOrCrit);
	DOREPLIFETIME(UGridCharacterAnimInstance, damage);
}

void UGridCharacterAnimInstance::WeaponAttack_Implementation()
{
	if(!bWeaponAttack)
		bWeaponAttack = true;
}

void UGridCharacterAnimInstance::GotHit_Implementation(bool hitOrCrit_)
{
	bHitOrCrit = hitOrCrit_;
	if (!bHit)
		bHit = true;
}

void UGridCharacterAnimInstance::SetDamage_Implementation(int damage_)
{
	damage = damage_;
}

void UGridCharacterAnimInstance::SkillAttack_Implementation(int index_)
{
	if(skillIndex == -1)
		skillIndex = index_;
}

void UGridCharacterAnimInstance::SetUseItem_Implementation()
{
	if(!useItem)
		useItem = true;
}

void UGridCharacterAnimInstance::DeathAnim_Implementation()
{
	if (!bDeath)
		bDeath = true;
}

void UGridCharacterAnimInstance::ChangeStats_Implementation(bool stat_)
{
	if (stat_) //Increase. Called by the player
	{
		if(!useItem)
			useItem = true;
	}
	else //Decrease called by the enemies
	{
		UE_LOG(LogTemp, Warning, TEXT("Decreasing stats animation"));
		if (!bStatDecrease)
			bStatDecrease = true;
	}
}

void UGridCharacterAnimInstance::GotMiss_Implementation()
{
	if (!bMiss)
		bMiss = true;
}