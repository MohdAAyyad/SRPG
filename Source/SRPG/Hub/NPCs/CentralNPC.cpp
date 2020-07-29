// Fill out your copyright notice in the Description page of Project Settings.


#include "CentralNPC.h"
#include "Components/WidgetComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"

void ACentralNPC::BeginPlay()
{
	Super::BeginPlay();
	// start the chance of success at a random number between 35/65
	chanceOfSuccess = FMath::RandRange(35, 65);

	activityEndLine = "Activity Failed";
	activityAlreadyDone = false;
}

void ACentralNPC::UpdateChanceOfSuccess(float value_)
{
	// adds to our current chance of success value by the input value
	chanceOfSuccess += value_;
}

int ACentralNPC::GetCentralActivityIndex()
{
	return activityIndex;
}

bool ACentralNPC::IsActivityAffordable()
{
	// sinces prices / money doesn't work yet it's always going to return true
	return true;
}

void ACentralNPC::SpendCost()
{
	if (IsActivityAffordable())
	{
		SimulateActivity();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough money to activate event"));
	}
}

void ACentralNPC::SimulateActivity()
{

	// spend time regardless of weather or not we succeed
	SpendTime();

	float result = FMath::FRandRange(0, 100);
	// if the result is within the chance of success
	if (result <= chanceOfSuccess)
	{
		activityEndLine = "Activity Success!";
		switch (rewardIndex)
		{
			case 0:
				// augment the money of the player
				break;
			case 1:
				// augment a stat of the player
				break;
			case 2:
				//decrement a stat of the enemy
				break;
			case 3:
				// augment the crowd value
				break;
		}
	}

	// regardless of the result we have already done the activity
	activityAlreadyDone = false;
}

void ACentralNPC::SpendTime()
{
	// empty function because we don't have a hub world manager to handle time
}

void ACentralNPC::EndDialogue()
{
	UE_LOG(LogTemp, Warning, TEXT("UnInteracted!"));
	if (widget)
	{
		widget->GetUserWidgetObject()->RemoveFromViewport();
	}
	line = FString("");
}

void ACentralNPC::LoadText()
{
	//leave this empty as we don't want text loading when we interact
}
