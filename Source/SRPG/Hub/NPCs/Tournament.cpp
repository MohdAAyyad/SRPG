// Fill out your copyright notice in the Description page of Project Settings.


#include "Tournament.h"
#include "Components/WidgetComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Intermediary/Intermediate.h"
#include "SRPGCharacter.h"
#include "Hub/HubWorldManager.h"


ATournament::ATournament():ACentralNPC()
{
	moneyCost = 0;
	op1SuccessChance = 0;
	op2SuccessChance = 0;
	hasBetOnOpponent = false;
	winner = false;
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
}

void ATournament::EndDialogue()
{
	//UE_LOG(LogTemp, Warning, TEXT("UnInteracted!"));
	if (widget)
		widget->GetUserWidgetObject()->RemoveFromViewport();
}

void ATournament::SetMoneyCost(int cost_, bool op_)
{
	//In the UI, make sure that currentMoney - cost_ >= 0 before calling this function. The check below should be just a formality
	// true = op1, false = op2

	// cost check for money here when functionality is added
	if (cost_ > 0)
	{
		if (currentMoney - cost_ >= 0)
		{
			moneyCost = cost_;

			if (op_)
			{
				hasBetOnOpponent = true;
			}
			else
			{
				hasBetOnOpponent = false;
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
	if (op1_)
	{
		op1SuccessChance += 20;
		op2SuccessChance -= 20;
	}
	else
	{
		op1SuccessChance -= 20;
		op2SuccessChance += 20;
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
		}
		else
		{
			winner = false;
		}

		// If we bet and won
		if (moneyCost != 0 && hasBetOnOpponent == winner)
		{
			// award the player 1.5 * money cost
			Intermediate::GetInstance()->SpendMoney(-(moneyCost * 1.5));
			moneyCost = 0;
		}
		activityAlreadyDone = true;

	}
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
