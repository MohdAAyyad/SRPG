// Fill out your copyright notice in the Description page of Project Settings.


#include "DayMenuCtrl.h"
#include "Intermediary/Intermediate.h"
#include "Components/WidgetComponent.h"

// Sets default values
ADayMenuCtrl::ADayMenuCtrl()
{
	PrimaryActorTick.bCanEverTick = true; //True, needed for the UI

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	RootComponent = root;

	dayMenuWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));

}

// Called when the game starts or when spawned
void ADayMenuCtrl::BeginPlay()
{
	Super::BeginPlay();

	if (dayMenuWidget)
		if (dayMenuWidget->GetUserWidgetObject())
			dayMenuWidget->GetUserWidgetObject()->AddToViewport();
	
}

int ADayMenuCtrl::GetCurrentDay()
{
	return Intermediate::GetInstance()->GetCurrentDay();
}

