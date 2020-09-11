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
	spentUnits = 0;
}

void ACentralNPC::OnOverlapWithPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OverlappedComp != nullptr)
	{
		// check if we are being interacted with and process the logic 
		if (interactedWith)
		{
			ASRPGCharacter* player = Cast<ASRPGCharacter>(OtherActor);
			if (player)
			{
				if (widget && activityAlreadyDone == false)
				{
					widget->GetUserWidgetObject()->AddToViewport();
					UE_LOG(LogTemp, Warning, TEXT("Added Widget To viewport"));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Widget is NULL"));
				}
			}
		}
	}
}

void ACentralNPC::PutUnitOnHold(int index_)
{
	if (spentUnits <= unitCost)
	{
		Intermediate::GetInstance()->PutUnitOnHold(index_);
		spentUnits += 1;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Too many units spent!"));
	}

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
	if (Intermediate::GetInstance()->GetCurrentMoney() - moneyCost > 0 && hubManager->GetCurrentTimeSlotsCount() > timeCost && spentUnits == unitCost)
	{
		Intermediate::GetInstance()->SpendMoney(moneyCost);
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
			UE_LOG(LogTemp, Warning, TEXT("Not enough money or units actived to activate event"));
		}
	}
	
}

void ACentralNPC::SimulateActivity()
{

	// spend time regardless of weather or not we succeed
	SpendTime();
	spentUnits = 0;

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
				// augment the money of the player, the amount is arbitrary but we can set it later
				// this case is specifically for Russian Roulette 
				Intermediate::GetInstance()->AddMoney(Intermediate::GetInstance()->GetCurrentMoney() * 1.25f);
				UE_LOG(LogTemp, Warning, TEXT("Money Added!"));
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
					case STAT_HP:
						row2.hp += 1;
						break;
					case STAT_PIP:
						row2.pip += 1;
						break;
					case STAT_ATK:
						row2.atk += 1;
						break;
					case STAT_DEF:
						row2.def += 1;
						break;
					case STAT_INT:
						row2.intl += 1;
						break;
					case STAT_SPD:
						row2.spd += 1;
						break;
					case STAT_CRT:
						row2.crit += 1;
						break;
					case STAT_HIT:
						row2.agl += 1;
						break;
					case STAT_CRD:
						row2.crd += 1;
						break;
					}
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Stats added!"));
				break;
			case 2:
				//decrement a stat of the enemy
			{
				UE_LOG(LogTemp, Warning, TEXT("Enemy Stat went down!"));
				int stat = FMath::RandRange(STAT_HP, STAT_CRD);
				Intermediate::GetInstance()->EnemyStatsGoDown(1, stat);
			}
				break;
			case 3:
				// augment the crowd value
			{
				UE_LOG(LogTemp, Warning, TEXT("CRD value improved"));
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
	spentUnits = 0;
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
