// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CutsceneActor.generated.h"

UCLASS()
class SRPG_API ACutsceneActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACutsceneActor();

	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMeshComponent* mesh;

	UPROPERTY(EditAnywhere, Category = "Root")
	USceneComponent* root;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TArray<class USkeletalMesh*> meshes;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	USkeletalMesh* GetMesh();

	void SetMesh(USkeletalMesh* mesh_);

	void PlayAnimation(class UAnimationAsset* anim_);

	void DeleteActor();

	

};
