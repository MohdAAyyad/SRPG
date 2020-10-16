// Fill out your copyright notice in the Description page of Project Settings.


#include "Tournament.h"
#include "Components/WidgetComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Intermediary/Intermediate.h"
#include "SRPGCharacter.h"
#include "Hub/HubWorldManager.h"
#include "Components/SkeletalMeshComponent.h"


ATournament::ATournament():ACentralNPC()
{
	moneyCost = 0;
	op1SuccessChance = 0;
	op2SuccessChance = 0;
	hasBetOnOpponent = false;
	winner = false;
	PrimaryActorTick.bCanEverTick = false;
}
void ATournament::BeginPlay()
{
	Super::BeginPlay();
	// generate the chances of success
	op1SuccessChance = FMath::RandRange(35, 65);
	op2SuccessChance = 100 - op1SuccessChance;

	//Create the two possible opponents
	op1 = FOpponentStruct();
	op1.InitStruct(Intermediate::GetInstance()->GetProtagonistLevel(), Intermediate::GetInstance()->GetCurrentRosterSize());
	op2 = FOpponentStruct();
	op2.InitStruct(Intermediate::GetInstance()->GetProtagonistLevel(), Intermediate::GetInstance()->GetCurrentRosterSize());

	hasSupportedTeam = false;
	spentTournamentUnits = 0;
	spentUnits = 0;
	timeCost = 0;
}

void ATournament::EndDialogue()
{
	//UE_LOG(LogTemp, Warning, TEXT("UnInteracted!"));
	if (widget)
	{
		widget->GetUserWidgetObject()->RemoveFromViewport();
		spentTournamentUnits = 0;
	}

}

void ATournament::SetMoneyCost(int cost_, bool op_)
{
	//In the UI, make sure that currentMoney - cost_ >= 0 before calling this function. The check below should be just a formality
	// true = op1, false = op2

	// cost check for money here when functionality is added
	if (cost_ > 0)
	{
		if (Intermediate::GetInstance()->GetCurrentMoney() - cost_ >= 0)
		{
			moneyCost = cost_;

			if (op_)
			{
				betTeam = true;
				hasBetOnOpponent = true;
			}
			else
			{
				betTeam = false;
				hasBetOnOpponent = true;
			}

			SpendCost();
		}
	}	
}

void ATournament::SpendCost()
{
	Intermediate::GetInstance()->SpendMoney(moneyCost);
	SpendTime();
}

void ATournament::SupportTeam(bool op1_)
{
	//TODO
	//Put units on hold on the intermediate
	if (hasSupportedTeam == false && spentTournamentUnits == 2)
	{
		if (op1_)
		{
			op1SuccessChance += 5;
			op2SuccessChance -= 5;
			// put the unit selected on hold
			//PutUnitOnHold();
			hasSupportedTeam = true;
			hasBetOnOpponent = true;
			UE_LOG(LogTemp, Warning, TEXT("Supported OP1"));
		}
		else
		{
			op1SuccessChance -= 5;
			op2SuccessChance += 5;
			UE_LOG(LogTemp, Warning, TEXT("Supported OP2"));
			hasSupportedTeam = true; 
			hasBetOnOpponent = false;
		}
	}

}

FOpponentStruct ATournament::SimulateMatch()
{
	if (!activityAlreadyDone)
	{
		int winningResult = FMath::RandRange(0, 100);
		// simulate the match and generate the results
		if (winningResult <= op1SuccessChance)
		{
			winner = true;
			UE_LOG(LogTemp, Warning, TEXT("OP1 Won"));
		}
		else
		{
			winner = false;
			UE_LOG(LogTemp, Warning, TEXT("OP2 Won"));
		}

		// If we bet and won
		if (moneyCost != 0 && hasBetOnOpponent && betTeam == winner)
		{
			// award the player 1.5 * money cost
			Intermediate::GetInstance()->AddMoney(moneyCost * 1.5);
			UE_LOG(LogTemp, Warning, TEXT("Bet Won, money added"));
			moneyCost = 0;
		}
		else if (hasBetOnOpponent != winner && hasSupportedTeam && unitsOnHold.Num() > 0)
		{
			// lose the fighters you sacrificed

			for (int i : unitsOnHold)
			{
				Intermediate::GetInstance()->UpdateCurrentRosterSize(-1);
				FName converted = *FString::FromInt(i);
				fileReader->RemoveFighterTableRow(converted, fileReader->FindTableIndexInArray(FName("FighterTableStruct")));
			}
			// clear out the array
			unitsOnHold.Empty();
		}
		activityAlreadyDone = true;

	}

	hasSupportedTeam = false;

	if (winner)
		return op1;

	return op2;
}

int ATournament::GetOp1Chance()
{
	return op1SuccessChance;
}

int ATournament::GetOp2Chance()
{
	return op2SuccessChance;
}

void ATournament::EnterFighterIndex(int index_)
{
	if (index_ <= Intermediate::GetInstance()->GetCurrentRosterSize() && index_ > -1)
	{
		fighterIndex = index_;
	}

}

void ATournament::OnOverlapWithPlayer(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OverlappedComp != nullptr)
	{
		// check if we are being interacted with and process the logic 
		if (interactedWith)
		{
			ASRPGCharacter* player = Cast<ASRPGCharacter>(OtherActor);
			if (player)
			{
				if (widget && activityAlreadyDone == false && widget->GetUserWidgetObject()->IsInViewport() == false)
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

bool ATournament::GetHasSupportedTeam()
{
	return hasSupportedTeam;
}
