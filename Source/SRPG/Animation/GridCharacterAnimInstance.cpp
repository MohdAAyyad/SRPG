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
void UGridCharacterAnimInstance::UpdateAnimationProperties()
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
void UGridCharacterAnimInstance::WeaponAttack()
{
	if(!bWeaponAttack)
		bWeaponAttack = true;
}

void UGridCharacterAnimInstance::GotHit(bool hitOrCrit_)
{
	bHitOrCrit = hitOrCrit_;
	if (!bHit)
		bHit = true;
}

void UGridCharacterAnimInstance::SetDamage(int damage_)
{
	damage = damage_;
}

void UGridCharacterAnimInstance::SkillAttack(int index_)
{
	if(skillIndex == -1)
		skillIndex = index_;
}

void UGridCharacterAnimInstance::SetUseItem()
{
	if(!useItem)
		useItem = true;
}

void UGridCharacterAnimInstance::DeathAnim()
{
	if (!bDeath)
		bDeath = true;
}

void UGridCharacterAnimInstance::ChangeStats(bool stat_)
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

void UGridCharacterAnimInstance::GotMiss()
{
	if (!bMiss)
		bMiss = true;
}