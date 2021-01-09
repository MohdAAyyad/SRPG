// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hub/NPCs/CentralNPC.h"
#include "ExternalFileReader/FOpponentStruct.h"
#include "Tournament.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API ATournament : public ACentralNPC
{
	GENERATED_BODY()
protected:

	ATournament();

	// hubworld*
	// random between 35-65

	// this is test value to test if betting works correctly
	UPROPERTY(BlueprintReadOnly) //Represents current money obtained from the intermediate. Used by UI
	int currentMoney;

	UPROPERTY(BlueprintReadOnly) // How much money was won from bets
	int gainedMoney;

	UPROPERTY(BlueprintReadOnly)
	int op1SuccessChance;
	// 100 - op1 
	UPROPERTY(BlueprintReadOnly)
	int op2SuccessChance;
	// true op 1, false op 2
	bool hasBetOnOpponent;
	UPROPERTY(BlueprintReadOnly) //Used to know which opponent to display after the simulation is done
	bool winner;

	UPROPERTY(BlueprintReadOnly)
	FOpponentStruct op1;
	UPROPERTY(BlueprintReadOnly)
	FOpponentStruct op2;

	void BeginPlay() override;
	void EndDialogue() override;

	UFUNCTION(BlueprintCallable)
		void SetMoneyCost(int cost_, bool op_); /*Called from UI to update moneyCost when the player presses bet. Updates player
	PlacesBetOnThisOp and calls SpendCost().*/
	void SpendCost() override;
	UFUNCTION(BlueprintCallable)
		void SupportTeam(bool op1_); /*Called from UI.Spends Units and a time slot to increase op1(true) or op2(false) by 20 % */
	// which fighter have you chosen to support
	int fighterIndex;


	bool betTeam;
	bool hasSupportedTeam;
	
public:

	UFUNCTION(BlueprintCallable)
		FOpponentStruct SimulateMatch(); /*Checks the winning percentage of op1 and randomly generates a value between 0 and 100.
	If the value is less or equal to op1's chance then winner is true, otherwise false. If the player has a bet (moneyCost > 0) 
	and wins, give the player 1.5 times the bet. Called when all the timeslots are used up and the next opponent
	is not a story mission or when the player manually simulates the result by interacting with the tourney npc.
	After it finishes, calls intermdiate's SetNextOpponent. Turns bActivityHasAlreadyBeenDone to true*/
	UFUNCTION(BlueprintCallable)
	int GetOp1Chance();
	UFUNCTION(BlueprintCallable)
	int GetOp2Chance();

	UFUNCTION(BlueprintCallable)
	void EnterFighterIndex(int index_);

	virtual void OnOverlapWithPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
										 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
										 bool bFromSweep, const FHitResult& SweepResult) override;
	UFUNCTION(BlueprintCallable)
	bool GetHasSupportedTeam();

	UPROPERTY(BlueprintReadWrite)
	int spentTournamentUnits;

	UFUNCTION(BlueprintCallable)
	int GetCurrentMoney();

	//void PutUnitOnHold(int index_) override;

	//void EndDialogue() override;
};
