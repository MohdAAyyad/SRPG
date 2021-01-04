// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterShopDisplayedFighter.h"
#include "Components/SkeletalMeshComponent.h"
#include "../../Battle/Weapons/WeaponBase.h"

// Sets default values
AFighterShopDisplayedFighter::AFighterShopDisplayedFighter()
{
	PrimaryActorTick.bCanEverTick = false;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(root);
}

void AFighterShopDisplayedFighter::BeginPlay()
{
	Super::BeginPlay();
	if (weaponMeshes.Num() > 0)
	{
		if (weaponMeshes[0])
		{
			FName weaponSocketName = TEXT("WeaponSocket");
			wpn0 = GetWorld()->SpawnActor<AWeaponBase>(weaponMeshes[0]);
			wpn0->AttachToComponent(mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, weaponSocketName);
		}
	}

	if (weaponMeshes.Num() > 1)
	{
		if (weaponMeshes[1])
		{
			FName shieldSocketName = TEXT("ShieldSocket");
			wpn1 = GetWorld()->SpawnActor<AWeaponBase>(weaponMeshes[1]);
			wpn1->AttachToComponent(mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, shieldSocketName);
		}
	}
}


void AFighterShopDisplayedFighter::Die()
{
	if (wpn0)
		wpn0->Destroy();

	if (wpn1)
		wpn1->Destroy();

	Destroy();
}