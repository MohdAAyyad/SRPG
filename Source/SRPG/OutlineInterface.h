// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OutlineInterface.generated.h"

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
	virtual void ActivateOutline(bool value_) = 0;
	virtual void TargetedOutline() = 0;
};
