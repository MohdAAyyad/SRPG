// Fill out your copyright notice in the Description page of Project Settings.


#include "CentralNPC.h"
#include "Components/WidgetComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Hub/HubWorldManager.h"
#include "SRPGCharacter.h"
#include "Intermediary/Intermediate.h"
#include "Definitions.h"
#include "Hub/NPCs/NPCWanderComponent.h"

void ACentralNPC::BeginPlay()
{
	Super::BeginPlay();
	// start the chance of success at a random number between 35/65
	chanceOfSuccess = FMath::RandRange(35, 65);

	activityEndLine = "";
	activityAlreadyDone = false;
	spentUnits = 0;

	initialMoneyValue = Intermediate::GetInstance()->GetCurrentMoney();

	// activity index randomization code goes here

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
		unitsOnHold.Push(index_);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Too many units spent!"));
	}

}

TArray<FFighterTableStruct> ACentralNPC::GetAllAvailbleFighters()
{
	return fileReader->GetAllRecruitedFighters(fileReader->FindTableIndexInArray(FName("FighterTableStruct")));
	//return fileReader->GetAllRecruitedFighters(1);
}

TArray<UTexture*> ACentralNPC::GetTextureArray()
{
	return fighterTextureArray;
}

bool ACentralNPC::ShouldAddUnitSacrificeUI()
{
	if(unitCost > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

FString ACentralNPC::WarningText(int warningType_)
{
	switch (warningType_)
	{
		//units too high
		case 1:
			return FString("Too Many Units Selected");
			break;
		//units too low
		case 2:
			return FString("Not Enough Units Selected");
			break;
		// not enough money
		case 3:
			return FString("Not Enough Money");
			break;
		// not enough money and too few units
		case 4:
			return FString("Not Enough Units Selected. Not Enough Money");
			break;
		// too many units not enough money
		case 5:
			return FString("Too Many Units Selected. Not Enough Money");
			break;
		// reset the warning string
		case 6:
			warning = FString();
			return warning;
			break;

	}
	
	return FString();
}

FString ACentralNPC::GetWarningText()
{
	return warning;
}

void ACentralNPC::UpdateSpentUnits(int value_)
{
	spentUnits += value_;
}

void ACentralNPC::SetSpentUnits(int value_)
{
	spentUnits = value_;
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
			warning = FString();
		}
		else
		{
			if (spentUnits > unitCost && Intermediate::GetInstance()->GetCurrentMoney() - moneyCost <= 0)
			{
				warning = WarningText(5);
			}
			else if (spentUnits < unitCost && Intermediate::GetInstance()->GetCurrentMoney() - moneyCost <= 0)
			{
				warning = WarningText(4);
			}
			else if (spentUnits > unitCost && Intermediate::GetInstance()->GetCurrentMoney() - moneyCost > 0)
			{
				warning = WarningText(1);
			}
			else if (spentUnits < unitCost && Intermediate::GetInstance()->GetCurrentMoney() - moneyCost > 0)
			{
				warning = WarningText(2);
			}
			else if (spentUnits == unitCost && Intermediate::GetInstance()->GetCurrentMoney() - moneyCost <= 0)
			{
				warning = WarningText(3);
			}
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
		// if there are any units on hold and we've succeeded, clear out the array 
		if (unitsOnHold.Num() > 0)
		{
			unitsOnHold.Empty();
		}
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
				Intermediate::GetInstance()->ChangeStats(1, stat);
				
			}
			UE_LOG(LogTemp, Warning, TEXT("Stats added!"));
				break;
			case 2:
				//decrement a stat of the enemy
			{
				UE_LOG(LogTemp, Warning, TEXT("Enemy Stat went down!"));
				int stat = FMath::RandRange(STAT_HP, STAT_CRD);
				Intermediate::GetInstance()->ChangeStats(0, stat);
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
		if (unitsOnHold.Num() > 0)
		{
			for (int i : unitsOnHold)
			{
				Intermediate::GetInstance()->UpdateCurrentRosterSize(-1);
				FName converted = *FString::FromInt(i);
				fileReader->RemoveFighterTableRow(converted, fileReader->FindTableIndexInArray(FName("FighterTableStruct")));
			}
			// clear out the array
			unitsOnHold.Empty();
		}
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
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Not enough time slots!"));
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
