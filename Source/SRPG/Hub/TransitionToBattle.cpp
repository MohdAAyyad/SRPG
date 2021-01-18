// Fill out your copyright notice in the Description page of Project Settings.


#include "TransitionToBattle.h"
#include "NPCs/Tournament.h"
#include "Particles/ParticleSystemComponent.h"
#include "SRPGGameMode.h"
#include "SRPGCharacter.h"
#include "Intermediary/Intermediate.h"
#include "ExternalFileReader/FOpponentStruct.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Kismet/GameplayStatics.h"
#include "Definitions.h"
#include "SRPGPlayerController.h"

// Sets default values
ATransitionToBattle::ATransitionToBattle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	particles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	particles->SetupAttachment(root);

	box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	box->SetGenerateOverlapEvents(true);
	box->SetCollisionProfileName("NPCDialogue");
	box->SetupAttachment(root);

	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	widgetComp->SetupAttachment(root);
	widgetComp->SetVisibility(false);

	fileReader = CreateDefaultSubobject<UExternalFileReader>(TEXT("File Reader"));
	
	gameMode = nullptr;
	tourney = nullptr;
	bHasBeenInteractedWith = false;
	bOnlinePlay = false;
}

// Called when the game starts or when spawned
void ATransitionToBattle::BeginPlay()
{
	Super::BeginPlay();

	if (box)
		box->OnComponentBeginOverlap.AddDynamic(this, &ATransitionToBattle::OverlapWithPlayer);

	gameMode = Cast<ASRPGGameMode>(GetWorld()->GetAuthGameMode());

	if (fileReader)
		dayInfo = fileReader->GetCurrentDayInfo(1, Intermediate::GetInstance()->GetCurrentDay());
	
}

// Called every frame
void ATransitionToBattle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ATransitionToBattle::OverlapWithPlayer(UPrimitiveComponent* overlappedComponent_,
											AActor* otherActor_,
											UPrimitiveComponent* otherComp_,
											int32 otherBodyIndex_,
											bool bFromSweep_,
											const FHitResult &sweepResult_)
{
	if (bHasBeenInteractedWith)
	{
		if (otherActor_ != nullptr && otherActor_ != this && otherComp_ != nullptr)
		{
			ASRPGCharacter* pChar = Cast<ASRPGCharacter>(otherActor_);

			if (pChar)
			{
				UpdateRecruitedFighters();
				if (widgetComp)
					widgetComp->GetUserWidgetObject()->AddToViewport();
			}
		}
	}
}

void ATransitionToBattle::Interact()
{
	bHasBeenInteractedWith = true;
}
void ATransitionToBattle::UnInteract()
{
	bHasBeenInteractedWith = false;

	if (widgetComp)
		widgetComp->GetUserWidgetObject()->RemoveFromViewport();
}

void ATransitionToBattle::EndDay()
{
	//Called when player presses on End Day in the UI
	//Gets the winner from tournament NPC and passes it to the intermediate

	if (tourney)
		Intermediate::GetInstance()->SetNextOpponent(tourney->SimulateMatch());

	ASRPGPlayerController* control = Cast<ASRPGPlayerController>(GetWorld()->GetFirstPlayerController());
	if (control)
	{
		control->SetInputMode(FInputModeUIOnly());
	}
}

void ATransitionToBattle::UpdateRecruitedFighters() //Called when the player collides with the actor
{
	if (recruitedFighters.Num() > 0) //Need to reset the array in case the player buys new fighters then returns
		recruitedFighters.Empty();

	if (fileReader)
	{
		TArray<FFighterTableStruct> rfighters = fileReader->GetAllRecruitedFighters(0);

		for (auto r : rfighters)
			recruitedFighters.Push(r);
	}
}

TArray<FFighterTableStruct> ATransitionToBattle::GetRecruitedFighters() //Called by the UI to display the fighters
{
	return recruitedFighters;
}

void ATransitionToBattle::RemoveRecruitedFighterAtIndex(int index_)
{
	if (indexesOfSelectedFighters.Contains(index_))
	{
		indexesOfSelectedFighters.RemoveAt(indexesOfSelectedFighters.IndexOfByKey(index_));
	}
}

void ATransitionToBattle::AddFighterToSelectedFighters(int index_)
{
	//Stores the index of the array entry of recruited fighters
	if (indexesOfSelectedFighters.Num() < Intermediate::GetInstance()->GetMaxDeploymentSize())
	{
		//Should check whether the array contains in the index first, but we'll do that through the UI by changing the add button to remove and have it call a different function
		if (index_ >= 0 && index_ < recruitedFighters.Num())
			indexesOfSelectedFighters.Push(index_);
	}
}

void ATransitionToBattle::FinalizeFighterSelection(bool online_)
{
	//Pass in the BPIDs to the intermediate
	for (int i = 0; i < indexesOfSelectedFighters.Num(); i++)
	{
		if(indexesOfSelectedFighters[i] >= 0 && indexesOfSelectedFighters[i] < recruitedFighters.Num())
			Intermediate::GetInstance()->AddFighterToSelected(recruitedFighters[indexesOfSelectedFighters[i]]);
	}

	if (widgetComp)
		widgetComp->GetUserWidgetObject()->RemoveFromViewport();


	if (!bOnlinePlay)
	{
		if (gameMode)
			gameMode->SwitchLevel(MAP_BATTLE);
	}

	if (widgetComp)
		widgetComp->GetUserWidgetObject()->RemoveFromViewport();
	/*if (gameMode)
		gameMode->SwitchLevel(MAP_BATTLE);*/

	if (online_)
	{
		// add in the "listen" command so it sets up a listen server
		UGameplayStatics::OpenLevel(GetWorld(), MAP_BATTLE, true, "?listen");
	}
	else if(gameMode) 
	{
			gameMode->SwitchLevel(MAP_BATTLE);
	}

}

void ATransitionToBattle::InitiateAReRun()
{
	Intermediate::GetInstance()->Defeat(dayInfo.retryMoneyCompensation, dayInfo.retryShardsCompensation);
}
