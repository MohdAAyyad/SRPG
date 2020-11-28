// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsObject.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SRPGCharacter.h"

// Sets default values
APhysicsObject::APhysicsObject()
{
 //	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	//root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	//RootComponent = root;

	//collider = CreateDefaultSubobject<USphereComponent>(TEXT("Box Collider"));
	//collider->SetupAttachment(RootComponent);

	//mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APhysicsObject::BeginPlay()
{
	/*Super::BeginPlay();
	force = FVector(0, 0, 0);*/
}

void APhysicsObject::UpdatePhysics(float deltaTime_)
{
	//acceleration.X = force.X / mass;
	//acceleration.Y = grav / mass;
	//acceleration.Z = force.Z / mass;

	///*force.X -= friction;
	//force.Y -= friction;*/

	//velocity += acceleration * deltaTime_;

	//SetActorLocation(GetActorLocation() + (velocity * deltaTime_));
}

void APhysicsObject::OnOverlapWithPlayer(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	/*if (OtherActor != nullptr && OtherActor != this && OverlappedComp != nullptr)
	{
		ASRPGCharacter* player = Cast<ASRPGCharacter>(OtherActor);
		if (player)
		{
			FVector ballRot = GetActorLocation();
			FVector playerRot = OtherActor->GetActorLocation();

			FVector result = playerRot - ballRot;
			result.Normalize();

			force = hitForce * result;
		}
	}*/
}

// Called every frame
void APhysicsObject::Tick(float DeltaTime)
{
	/*Super::Tick(DeltaTime);

	UpdatePhysics(DeltaTime);*/
}

