// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsObject.generated.h"

UCLASS()
class SRPG_API APhysicsObject : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APhysicsObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	int mass;
	FVector force;
	UPROPERTY(EditAnywhere)
	FVector hitForce;
	UPROPERTY(EditAnywhere)
	float friction;
	UPROPERTY(EditAnywhere)
	int grav;
	FVector acceleration;
	FVector velocity;

	void UpdatePhysics(float deltaTime_);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* collider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root")
	class USceneComponent* root;


	UFUNCTION()
		virtual void OnOverlapWithPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
										 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
										 bool bFromSweep, const FHitResult& SweepResult);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
