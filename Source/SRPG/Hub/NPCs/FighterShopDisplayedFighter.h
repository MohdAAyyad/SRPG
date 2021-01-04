// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FighterShopDisplayedFighter.generated.h"

UCLASS()
class SRPG_API AFighterShopDisplayedFighter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFighterShopDisplayedFighter();
	UPROPERTY(EditAnywhere, Category = "Root")
		USceneComponent* root;
	UPROPERTY(EditAnywhere, Category ="Mesh")
		USkeletalMeshComponent* mesh;

	UPROPERTY(EditAnywhere, Category = "Weapon")
		TArray<TSubclassOf<class AWeaponBase>> weaponMeshes;

	void Die();

protected:

	void BeginPlay() override;

	AWeaponBase* wpn0;
	AWeaponBase* wpn1;
};
