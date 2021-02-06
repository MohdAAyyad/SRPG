// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdProjectile.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "../Grid/Obstacle.h"
#include "Definitions.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ACrowdProjectile::ACrowdProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	sphere->SetupAttachment(root);
	sphere->SetGenerateOverlapEvents(true);
	sphere->SetCollisionProfileName("CrowdProjectile");

	particles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	particles->SetupAttachment(root);

	movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	movement->SetUpdatedComponent(root);

	statusEffect = EFFECT_NONE;


}

void ACrowdProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (sphere)
		sphere->OnComponentBeginOverlap.AddDynamic(this, &ACrowdProjectile::OverlapWithObstacle);
}

void ACrowdProjectile::OverlapWithObstacle(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OverlappedComp != nullptr && OtherActor != this)
	{
		//Check for a collision with an obstacle; damage it and apply a status effect, and spawn the emitter then die
		AObstacle* obst = Cast<AObstacle>(OtherActor);
		if (obst)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), emitter, GetActorLocation(), FRotator::ZeroRotator);
			obst->ObstacleTakeDamage(20.0f, statusEffect);
			//TODO, play sound effect
			Destroy();

		}
	}
}

