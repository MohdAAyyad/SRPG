// Fill out your copyright notice in the Description page of Project Settings.


#include "Tournament.h"
#include "Components/WidgetComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "SRPGCharacter.h"

void ATournament::BeginPlay()
{
	Super::BeginPlay();
	// generate the chances of success
	op1SuccessChance = FMath::RandRange(35, 65);
	op2SuccessChance = 100 - op1SuccessChance;

	moneyCost = 0;
}

void ATournament::EndDialogue()
{
	UE_LOG(LogTemp, Warning, TEXT("UnInteracted!"));
	if (widget)
	{
		widget->GetUserWidgetObject()->RemoveFromViewport();
	}
}

void ATournament::TwoOpponents(int protagonistLevel_)
{

}

void ATournament::SetMoneyCost(int cost_, bool op_)
{
	// true = op1, false = op2

	// cost check for money here when functionality is added
	if (cost_ > 0)
	{
		if (betMoney - cost_ >= 0)
		{
			moneyCost = cost_;

			if (op_)
			{
				hasBetOnOpponent = true;
				UE_LOG(LogTemp, Warning, TEXT("Bet on opponent 1"));
				//op1SuccessChance
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Bet on opponent 2"));
				hasBetOnOpponent = false;
			}

			SpendCost();
		}
		else
		{
			// not enough money
			UE_LOG(LogTemp, Warning, TEXT("Not enough money"));
		}
	}
	

	
}

void ATournament::SpendCost()
{
	//moneyCost - money
	SpendTime();
}

void ATournament::SupportTeam(bool op1_)
{
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

void ATournament::SimulateMatch()
{
	int winningResult = FMath::RandRange(0, 100);
	// simulate the match and generate the results
	if (winningResult <= op1SuccessChance)
	{
		winner = true;
		UE_LOG(LogTemp, Warning, TEXT("Opponent 1 Won"));
	}
	else
	{
		winner = false;
		UE_LOG(LogTemp, Warning, TEXT("Opponent 2 Won"));
	}

	// if we bet and won
	if (moneyCost != 0 && hasBetOnOpponent == winner)
	{
		// award the player 1.5 * money cost
		betMoney = betMoney + moneyCost * 1.5;
		moneyCost = 0;
		UE_LOG(LogTemp, Warning, TEXT("Won the bet!"));
	}

	// regenerate the values for the next competition
	op1SuccessChance = FMath::RandRange(35, 65);
	op2SuccessChance = 100 - op1SuccessChance;
	moneyCost = 0;
	UE_LOG(LogTemp, Warning, TEXT("Regenerated Numbers"));
}
