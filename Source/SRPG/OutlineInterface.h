// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OutlineInterface.generated.h"

USTRUCT(BlueprintType)
struct SRPG_API FHoverInfo
{
	GENERATED_USTRUCT_BODY()
public:
	FHoverInfo()
	{
		targetIndex = -1; //0 for player, 1 for enemy, and 2 for obstacles
		hpPercentage = pipPercentage = 0.0f;
		textureID = -1;
	}
	UPROPERTY(BlueprintReadWrite)
		int targetIndex;
	UPROPERTY(BlueprintReadWrite)
		float hpPercentage;
	UPROPERTY(BlueprintReadWrite)
		float pipPercentage;
	UPROPERTY(BlueprintReadWrite)
		int textureID;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOutlineInterface : public UInterface
{
	GENERATED_BODY()
};

class SRPG_API IOutlineInterface
{
	GENERATED_BODY()

public:
	inline virtual void ActivateOutline(bool value_) {};
	inline virtual void TargetedOutline() {};
	inline virtual FHoverInfo GetMyHoverInfo() { return FHoverInfo(); };
protected:
	FHoverInfo myHoverInfo;
};
