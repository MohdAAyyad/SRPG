// Fill out your copyright notice in the Description page of Project Settings.


#include "CentralNPC.h"
#include "Components/WidgetComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Hub/HubWorldManager.h"
#include "SRPGCharacter.h"
#include "Intermediary/Intermediate.h"
#include "Definitions.h"

void ACentralNPC::BeginPlay()
{
	Super::BeginPlay();
	// start the chance of success at a random number between 35/65
	chanceOfSuccess = FMath::RandRange(35, 65);

	activityEndLine = "";
	activityAlreadyDone = false;
}

void ACentralNPC::UpdateChanceOfSuccess(float value_)
{
	// adds to our current chance of success value by the input value
	UE_LOG(LogTemp, Warning, TEXT("Updated Chance of Success by %f"), value_);
	chanceOfSuccess += value_;
}

int ACentralNPC::GetCentralActivityIndex()
{
	return activityIndex;
}

bool ACentralNPC::IsActivityAffordable()
{
	// see if we can both afford the money cost and the time cost
	if (Intermediate::GetInstance()->GetCurrentMoney() > 0 && hubManager->GetCurrentTimeSlotsCount() > timeCost)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ACentralNPC::SpendCost()
{
	if (activityAlreadyDone)
	{
		line = "Sorry bud you've already done this before";
	}
	else
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
	
}

void ACentralNPC::SimulateActivity()
{

	// spend time regardless of weather or not we succeed
	SpendTime();

	float result = FMath::FRandRange(0, 100);
	// if the result is within the chance of success
	if (result <= chanceOfSuccess)
	{
		// success state
		FActivityDialogueTableStruct row = fileReader->GetPositiveCentral(activityIndex, 0);
		line = row.dialogue;
		activityEndLine = "Activity Success!";
		// lines would be loaded in via file but for now is just just predetermined
		switch (rewardIndex)
		{
			case 0:
				// augment the money of the player
				Intermediate::GetInstance()->AddMoney(FMath::RandRange(200, 300));
				break;
			case 1:
			{
				// choose a stat to give one of to each party member
				int stat = FMath::RandRange(STAT_HP, STAT_CRD);

				// augment a stat of the player
				UDataTable* table = fileReader->GetTable(1);
				TArray<FName> rowNames = table->GetRowNames();
				for (auto n : rowNames)
				{
					FFighterTableStruct row2 = fileReader->FindFighterTableRow(n, 1);
					switch (stat)
					{
					case 0:
						row2.hp += 1;
						break;
					case 1:
						row2.pip += 1;
						break;
					case 2:
						row2.atk += 1;
						break;
					case 3:
						row2.def += 1;
						break;
					case 4:
						row2.intl += 1;
						break;
					case 5:
						row2.spd += 1;
						break;
					case 6:
						row2.crit += 1;
						break;
					case 7:
						row2.agl += 1;
						break;
					case 8:
						row2.crd += 1;
						break;

					}
				}
			}
				break;
			case 2:
				//decrement a stat of the enemy
			{
				int stat = FMath::RandRange(STAT_HP, STAT_CRD);
				Intermediate::GetInstance()->EnemyStatsGoDown(1, stat);
			}
				break;
			case 3:
				// augment the crowd value
			{
				Intermediate::GetInstance()->ImprovePlayerCRD(FMath::RandRange(IMP_CRD_LW, IMP_CRD_HG));
			}
			break;
		}
	}
	else
	{
		// fail state
		FActivityDialogueTableStruct row = fileReader->GetNegativeCentral(activityIndex, 0);
		line = row.dialogue;
		//line = "Well that's a fail there";
		activityEndLine = "Activity Failed";
	}

	// regardless of the result we have already done the activity
	activityAlreadyDone = true;
}

void ACentralNPC::SpendTime()
{
	// empty function because we don't have a hub world manager to handle time
	// ^ comment aged well
		if (hubManager && hubManager->GetCurrentTimeSlotsCount() - timeCost > 0)
		{
			hubManager->UpdateTimeSlots(-timeCost);
		}
}

void ACentralNPC::EndDialogue()
{
	UE_LOG(LogTemp, Warning, TEXT("UnInteracted!"));
	if (widget)
	{
		widget->GetUserWidgetObject()->RemoveFromViewport();
	}
	//line = FString("");
}

FString ACentralNPC::GetChanceOfSuccessString()
{
	FString converted = FString::FromInt(chanceOfSuccess);
	return converted;
}

FString ACentralNPC::GetEndOfActivityLine()
{
	return activityEndLine;
}

void ACentralNPC::LoadText()
{
	//leave this empty as we don't want text loading when we interact
}
