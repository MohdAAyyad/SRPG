// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hub/NPC.h"
#include "BranchNPC.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API ABranchNPC : public ANPC
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
	class ACentralNPC* central;
	UPROPERTY(BlueprintReadOnly)
	bool hasUpdatedCentral;
	float chanceOfSuccessEffect;

	void EndDialogue() override;
	void LoadText() override;
	void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	FString information;

	virtual void OnOverlapWithPlayer(UPrimitiveComponent * overlappedComp_, AActor * otherActor_,
									 UPrimitiveComponent * otherComp_, int32 otherBodyIndex_,
									 bool bFromSweepO, const FHitResult & sweepResult_) override;

public:
	void SetCentralNPC(class ACentralNPC* ref_);
	void SetChanceOfSuccessEffect(float value_);
};
