// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGridCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectileGridCharacter::AProjectileGridCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	particles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	particles->SetupAttachment(root);

	movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	movement->SetUpdatedComponent(root);

}
