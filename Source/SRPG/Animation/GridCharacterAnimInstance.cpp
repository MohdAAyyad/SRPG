// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCharacterAnimInstance.h"
#include "../Battle/GridCharacter.h"


UGridCharacterAnimInstance::UGridCharacterAnimInstance()
{
	ownerGridCharacter = nullptr;
	forwardSpeed = 0.0f;
	bWeaponAttack = false;
	bHit = false;
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

void UGridCharacterAnimInstance::GotHit()
{
	if (!bHit)
		bHit = true;
}