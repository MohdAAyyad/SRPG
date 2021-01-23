// Fill out your copyright notice in the Description page of Project Settings.


#include "CutsceneActor.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ACutsceneActor::ACutsceneActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(root);


}

// Called when the game starts or when spawned
void ACutsceneActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACutsceneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

USkeletalMesh* ACutsceneActor::GetMesh()
{
	return mesh->SkeletalMesh;
}

void ACutsceneActor::SetMesh(USkeletalMesh* mesh_)
{
	mesh->SetSkeletalMesh(mesh_);
}

void ACutsceneActor::PlayAnimation(UAnimationAsset* anim_)
{
	mesh->PlayAnimation(anim_, true);
}

void ACutsceneActor::DeleteActor()
{
	Destroy();
}

