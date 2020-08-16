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
	
	gameMode = nullptr;
	tourney = nullptr;
	bHasBeenInteractedWith = false;
}

// Called when the game starts or when spawned
void ATransitionToBattle::BeginPlay()
{
	Super::BeginPlay();

	if (box)
		box->OnComponentBeginOverlap.AddDynamic(this, &ATransitionToBattle::OverlapWithPlayer);

	gameMode = Cast<ASRPGGameMode>(GetWorld()->GetAuthGameMode());
	
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
	{
		Intermediate::GetInstance()->SetNextOpponent(tourney->SimulateMatch());
		if(widgetComp)
			widgetComp->GetUserWidgetObject()->RemoveFromViewport();
		if (gameMode)
			gameMode->SwitchLevel(nextBattleLevel);
	}
}
