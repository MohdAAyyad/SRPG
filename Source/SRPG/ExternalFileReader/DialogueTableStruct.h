// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueTableStruct.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct SRPG_API FDialogueTableStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:


	FDialogueTableStruct()
	{

	}

	UPROPERTY(BlueprintReadWrite)
		FString line;


};
