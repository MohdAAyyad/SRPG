// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hub/NPC.h"
#include "CentralNPC.generated.h"

/**
 * 
 */
UCLASS()
class SRPG_API ACentralNPC : public ANPC
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
	int activityIndex;
	float chanceOfSuccess;//Starts at a random value between 35% and 65%. Affected by branch NPCs.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int moneyCost; // how much does it cost to do this activity?
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int unitCost; // how many units do you need to comit to this activity?
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int timeCost; // how many time slots does it cost to do this activity?
	UPROPERTY(EditAnywhere)
	int rewardIndex;//0 Money 1 Player stats 2 Enemy stats 3 CRD
	UPROPERTY(EditAnywhere)
	float successRewards; // affects a stat based on the reward index
	FString activityEndLine;// updated after activity succeeds or fails
	UPROPERTY(BlueprintReadOnly)
	bool activityAlreadyDone; // if true the percentage UI widget will not be added to the ui

	UFUNCTION(BlueprintCallable)
	bool IsActivityAffordable(); /*Checks if the player has enough 
	time slots and enough(money and or units) to afford the activity.
	Should be used in the UI widget to have the "Accept" button be enabled or disabled based on the result*/
	UFUNCTION(BlueprintCallable)
	virtual void SpendCost(); // overriden by children, spends the cost and calls simulate activity
	UFUNCTION(BlueprintCallable) 
	virtual void SimulateActivity(); // overriden by children, rewards the player based on the reward index and success index. Calls spend time
	
	void SpendTime(); //calls update time slot in hub manager and passes in the time cost. Must be called at the end of simulate activity unless otherwise specified
	void EndDialogue() override;

	UFUNCTION(BlueprintCallable)
	FString GetChanceOfSuccessString();
	UFUNCTION(BlueprintCallable)
	FString GetEndOfActivityLine();

	void LoadText() override;
	void BeginPlay() override;

	virtual void OnOverlapWithPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
										 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
										 bool bFromSweep, const FHitResult& SweepResult) override;
	UFUNCTION(BlueprintCallable)
	void PutUnitOnHold(int index_);
	UFUNCTION(BlueprintCallable)
	TArray<FFighterTableStruct> GetAllAvailbleFighters();
	UPROPERTY(BlueprintReadOnly)
	int spentUnits;
	UFUNCTION(BlueprintCallable)
	TArray<UTexture*> GetTextureArray();

	UPROPERTY(EditAnywhere)
	TArray<UTexture*> fighterTextureArray;

	TArray<int> unitsOnHold;

	UFUNCTION(BlueprintCallable)
	bool ShouldAddUnitSacrificeUI();

	UFUNCTION(BlueprintCallable)
	FString WarningText(int warningType_);
	UFUNCTION(BlueprintCallable)
	FString GetWarningText();

	UPROPERTY(EditAnywhere)
	FString warning;

	UFUNCTION(BlueprintCallable)
	void UpdateSpentUnits(int value_);
	UFUNCTION(BlueprintCallable)
	void SetSpentUnits(int value_);
public:

	void UpdateChanceOfSuccess(float value_);
	int GetCentralActivityIndex();
};
