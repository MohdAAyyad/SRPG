// Fill out your copyright notice in the Description page of Project Settings.


#include "FightersShop.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Definitions.h"
#include "Intermediary/Intermediate.h"
#include "FighterShopDisplayedFighter.h"
#include "Engine/World.h"
#include "SRPGCharacter.h"
#include "SRPGPlayerController.h"
#include "TimerManager.h"


AFightersShop::AFightersShop():ANPC()
{
	fighterID = 0;
	PrimaryActorTick.bCanEverTick = false;

	displayLocationOffset = FVector::ZeroVector;

}
void AFightersShop::BeginPlay()
{
	Super::BeginPlay();
	fighterID = Intermediate::GetInstance()->GetLatestFighterID(); //Assuming we do not have any fighters yet
	GetAllFightersForSale();

	if (Intermediate::GetInstance()->GetDeadUnits().Num() > 0)
	{
		if (fileReader)
		{
			fileReader->RemoveFightersDueToPermaDeath(Intermediate::GetInstance()->GetDeadUnits(), 1); //Remove any dead units from the recruited fighters table
		}
	}
	//Check if we have selected fighters at begin play. If we do, then save their new stats for level up purposes
		/*When the player decides to level up we save a copy of the SFighter
		so that when the player fluctuates between the levels the results no longer follow a random chance
		and are consistent i.e. this is used to make sure the random chance is only run once on level up.*/
	TArray<FFighterTableStruct> fighters;
	if (Intermediate::GetInstance()->GetSelectedFighters().Num() > 0)
	{
		fighters = Intermediate::GetInstance()->GetSelectedFighters();
		Intermediate::GetInstance()->ResetSelectedFighters();
	}
	if (fileReader)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gonna try it out"));
		//We need to need to check if we have recruited fighters and update the fighterID accordingyl
		TArray<FFighterTableStruct> allRecruitedFighters = fileReader->GetAllRecruitedFighters(1);
		if (allRecruitedFighters.Num() > 0)
		{
			fighterID = allRecruitedFighters[allRecruitedFighters.Num() - 1].id; //We already have fighters, so update the fighter id to match the id of the last fighter we have
			Intermediate::GetInstance()->SetLatestFighterID(fighterID);
		}

		if (fighters.Num() > 0)
		{
			for (int j = 0; j < fighters.Num(); j++)
			{
				fileReader->IncreaseTheStatsOfThisFigheter(fighters[j], 1); //Update the stats of these fighters in the recruited fighters table
			}

			fighters.Empty();
		}
	}
}

TArray<FFighterTableStruct> AFightersShop::GetAllFightersForSale()
{
	if (fileReader)
	{
		return fileReader->GetAllFighters(0);
	}
	
	return TArray<FFighterTableStruct>();
}


void AFightersShop::SelectFighterToBuy(FFighterTableStruct fighter_)
{
	chosenFighter = fighter_;	
}

void AFightersShop::UpdateChosenFighterName(FString name_)
{
	chosenFighter.name = name_;		
}

void AFightersShop::LevelUpFighter(int value_) //Value_ could be + or -
{
	if (value_ > 0)
	{
		chosenFighter.ScaleStatsByLevelUp(chosenFighter.level + 1);
	}
	else
	{
		if(chosenFighter.level - 1 > 0)
			chosenFighter.ScaleStatsByLevelDown(chosenFighter.level - 1);
	}
}

int AFightersShop::GetCurrentPrice() //Called from the UI after leveling up or down the fighter
{
	return chosenFighter.value;
}

void AFightersShop::FinalizePurchase()
{

	//The check to see if the fighter is affordable is done in the UI

	// load all of the chosen fighters values into the data table;
	Intermediate::GetInstance()->SpendMoney(chosenFighter.value);

	FFighterTableStruct row;
	row.bpid = chosenFighter.bpid;
	row.name = chosenFighter.name;
	row.level = chosenFighter.level;
	row.currentEXP = chosenFighter.currentEXP;
	row.neededEXPToLevelUp = chosenFighter.neededEXPToLevelUp;
	row.weaponIndex = chosenFighter.weaponIndex;
	row.armorIndex = chosenFighter.armorIndex;
	row.equippedWeapon = chosenFighter.equippedWeapon;
	row.equippedArmor = chosenFighter.equippedArmor;
	row.equippedAccessory = chosenFighter.equippedAccessory;
	row.hp = chosenFighter.hp;
	row.pip = chosenFighter.pip;
	row.atk = chosenFighter.atk;
	row.def = chosenFighter.def;
	row.intl = chosenFighter.intl;
	row.spd = chosenFighter.spd;
	row.crit = chosenFighter.crit;
	row.agl = chosenFighter.agl;
	row.crd = chosenFighter.crd;
	row.archetype = chosenFighter.archetype;
	row.id = ++fighterID; //Add 1 to fighter ID then make that equal to row id. Makes sure no two fighters have the same ID
	row.value = 0;

	// add the new row to the table
	FName rowName = FName(*FString::FromInt(row.id));
	fileReader->AddRowToRecruitedFighterTable(rowName, 1, row);
	// adds one to the roster size
	Intermediate::GetInstance()->UpdateCurrentRosterSize(1);
}
//Called from the UI
void AFightersShop::DisplayFighter(int index_)
{
	if (index_ >= 0 && index_ < fightersToDisplay.Num())
	{
		//Delete the currently displayedFighter and display the new one
		if (displayedFighter)
		{
			displayedFighter->Die();
			displayedFighter = nullptr;
		}
		FVector spawnLoc = GetActorLocation() + displayLocationOffset;
		displayedFighter = GetWorld()->SpawnActor<AFighterShopDisplayedFighter>(fightersToDisplay[index_], spawnLoc, GetActorRotation());
		displayedFighter->SetActorScale3D(displayScale);
	}
}

void AFightersShop::OnOverlapWithPlayer(UPrimitiveComponent * overlappedComp_, AActor * otherActor_,
	UPrimitiveComponent * otherComp_, int32 otherBodyIndex_,
	bool bFromSweepO, const FHitResult & sweepResult_)
{
	if (otherActor_ != nullptr && otherActor_ != this && overlappedComp_ != nullptr)
	{
		// check if we are being interacted with and process the logic 
		if (interactedWith)
		{
			ASRPGCharacter* player = Cast<ASRPGCharacter>(otherActor_);
			if (player)
			{
				playerRef = player;
				if (widget && widget->GetUserWidgetObject()->IsInViewport() == false)
				{
					//Focus the camera on the item shop
					ASRPGPlayerController* ctrl = Cast<ASRPGPlayerController>(GetWorld()->GetFirstPlayerController());
					if (ctrl)
					{
						float focusRate = 0.45f;
						ctrl->FocusOnThisNPC(this, focusRate);
						FTimerHandle timeToAddWidgetHandle;

						GetWorld()->GetTimerManager().SetTimer(timeToAddWidgetHandle, this, &ANPC::DelayedAddWidgetToViewPort, focusRate + 0.1f, false);
						ctrl->SetInputMode(FInputModeUIOnly());
					}
				}

			}
		}
	}
}


void AFightersShop::LeaveNPC()
{
	//Remove the widget from the viewport
	if (widget)
		if (widget->GetUserWidgetObject())
			widget->GetUserWidgetObject()->RemoveFromViewport();

	//Destroy the displayed fighter
	if (displayedFighter)
	{
		displayedFighter->Die();
		displayedFighter = nullptr;
	}

	//Move the camera back to the palyer
	ASRPGPlayerController* ctrl = Cast<ASRPGPlayerController>(GetWorld()->GetFirstPlayerController());
	if (ctrl)
	{
		if (playerRef)
		{
			ctrl->SetInputMode(FInputModeGameAndUI());
			ctrl->FocusOnThisNPC(playerRef, 0.45f);
			playerRef = nullptr;
		}
	}
}

void AFightersShop::RemoveDisplayedFighter()
{
	//Destroy the displayed fighter
	if (displayedFighter)
	{
		displayedFighter->Die();
		displayedFighter = nullptr;
	}
}
