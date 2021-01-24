// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/Grid/Obstacle.h"
#include "DestructibleObstacle.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API ADestructibleObstacle : public AObstacle
{
	GENERATED_BODY()
public:
	ADestructibleObstacle();
protected:

		UPROPERTY(EditAnywhere, Category = "Destructible Mesh")
		class UDestructibleComponent* mesh;

		UPROPERTY(EditAnywhere, Category = "Destructible Mesh")
			float maxHp;

		float hp;

		UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widget")
			class UWidgetComponent* widgetComp;

		void AddObstacleToObstacleManager() override;

public:

	void ObstacleTakeDamage(float damage_, int statusEffect_) override;
	void ActivateOutline(bool value_) override;
	void TargetedOutline() override;

	UFUNCTION(BlueprintCallable)
		float GetHealthPercentage();

	UPROPERTY(BlueprintReadWrite)
		bool bShowHealthBar;

	UPROPERTY(BlueprintReadWrite)
		float damageTaken;
};
