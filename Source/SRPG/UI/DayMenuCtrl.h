// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DayMenuCtrl.generated.h"

UCLASS()
class SRPG_API ADayMenuCtrl : public AActor
{
	GENERATED_BODY()
	
public:	
	ADayMenuCtrl();

protected:
	void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Root")
		USceneComponent* root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		class UWidgetComponent* dayMenuWidget;

	UFUNCTION(BlueprintCallable)
		int GetCurrentDay();

};
