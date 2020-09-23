// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCCharacterAnimInstance.h"

UNPCCharacterAnimInstance::UNPCCharacterAnimInstance()
{
	isWalking = false;
}

void UNPCCharacterAnimInstance::SetIsWalking(bool walking_)
{
	isWalking = walking_;
}
