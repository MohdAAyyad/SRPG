// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleObstacle.h"
#include "DestructibleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "TimerManager.h"

ADestructibleObstacle::ADestructibleObstacle():AObstacle()
{
	staticMesh->RemoveFromRoot();
	staticMesh->DestroyComponent();

	mesh = CreateDefaultSubobject< UDestructibleComponent>(TEXT("Destructible Mesh"));
	mesh->SetupAttachment(root);
	mesh->SetRenderCustomDepth(true);
	mesh->SetCustomDepthStencilValue(0);

	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	widgetComp->SetupAttachment(root);
	widgetComp->SetVisibility(false);

	maxHp = hp = 30.0f;

}
void ADestructibleObstacle::ObstacleTakeDamage(float damage_, int statusEffect_)
{
	bShowHealthBar = true;
	damageTaken = damage_;
	hp -= damage_;

	if (hp <= 0.5f)
	{
		FTimerHandle timeToDestroyHandle;
		mesh->ApplyDamage(5.0f, GetActorLocation(), FVector::ZeroVector, 1.0f);
		mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetWorld()->GetTimerManager().SetTimer(timeToDestroyHandle, this, &AObstacle::DelayedDestroy, 2.0f, false);
	}
}

void ADestructibleObstacle::ActivateOutline(bool value_)
{
	if (bCanBeTargeted)
	{
		if (value_)
			mesh->SetCustomDepthStencilValue(6);
		else
			mesh->SetCustomDepthStencilValue(0);
	}
}

void  ADestructibleObstacle::TargetedOutline()
{
	if (bCanBeTargeted)
	{
		mesh->SetCustomDepthStencilValue(2);
	}
}

float ADestructibleObstacle::GetHealthPercentage()
{
	return (hp / maxHp);
}