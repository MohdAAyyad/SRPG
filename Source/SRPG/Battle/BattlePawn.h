// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BattlePawn.generated.h"

UCLASS()
class SRPG_API ABattlePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABattlePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Root)
		USceneComponent* root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* mainCamera;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* cameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* CursorToWorld;

	class ABattleController* pController;
	bool bLockedOnToStaticActor;
	bool bLockedOnToCharacter;
	AActor* lockedOnTarget;
	float lockOnRate;
	float originalFOV;
	FVector targetDest;

	//Limits of the battle pawn movement range
	FVector maxMovement;
	FVector minMovement;

	//Final Destination
	class ABattleCrowd* btlCrowd;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void MoveUpDown(float rate_);
	void MoveRightLeft(float rate_);
	void Zoom(float rate_);
	void LockOnActor(AActor* target_);
	void LockOnActor(ABattleCrowd* btlCrd_, AActor* target_); //A different version used by the battle crowd. We need a ref to the btl crowd because we need to tell it when we've arrived
	void ResetLock();

	void SetMinMaxMovement(FVector minMovement_, FVector maxMovment_);
};
