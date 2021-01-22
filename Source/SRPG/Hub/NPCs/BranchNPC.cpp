// Fill out your copyright notice in the Description page of Project Settings.


#include "BranchNPC.h"
#include "Hub/NPCs/CentralNPC.h"
#include "Components/WidgetComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Hub/NPCs/NPCWanderComponent.h"
#include "SRPGPlayerController.h"
#include "SRPGCharacter.h"

void ABranchNPC::EndDialogue()
{
	//UE_LOG(LogTemp, Warning, TEXT("UnInteracted!"));
	if (widget->GetUserWidgetObject()->IsInViewport())
	{
		widget->GetUserWidgetObject()->RemoveFromViewport();


		ASRPGPlayerController* control = Cast<ASRPGPlayerController>(GetWorld()->GetFirstPlayerController());
		if (control)
		{
			control->SetInputMode(FInputModeGameAndUI());

			if (hasUpdatedCentral == false)
			{
				control->FocusOnThisNPC(control->GetPlayerReference(), 0.45f);
			}
		}

		hasUpdatedCentral = true;
	}
}

void ABranchNPC::LoadText()
{
	UE_LOG(LogTemp, Warning, TEXT("Load Text on Branch"));
	if (hasUpdatedCentral == false && central && chanceOfSuccessEffect >= 0)
	{
		//line = "I'm sure if you were to talk to the guy over there, things could go your way.";
		FActivityDialogueTableStruct row = fileReader->GetPositiveBranch(central->GetCentralActivityIndex(), 0);
		line = row.dialogue;
		//line = filedea
		central->UpdateChanceOfSuccess(chanceOfSuccessEffect);

		information = "Chance of success of an activity has risen.";

	}
	else if (hasUpdatedCentral == false && central && chanceOfSuccessEffect < 0)
	{
		FActivityDialogueTableStruct row = fileReader->GetNegativeBranch(central->GetCentralActivityIndex(), 0);
		line = row.dialogue;
		central->UpdateChanceOfSuccess(chanceOfSuccessEffect);

		information = "Chance of success of an activity has decreased.";
		
	}
	else if(hasUpdatedCentral == true)
	{
		line = "I've nothing else to say.";
	}
}

void ABranchNPC::BeginPlay()
{
	Super::BeginPlay();
	chanceOfSuccessEffect = FMath::RandRange(-35, 35);
	hasUpdatedCentral = false;
	line = "";
	information = "";
}

void ABranchNPC::OnOverlapWithPlayer(UPrimitiveComponent * overlappedComp_, AActor * otherActor_, UPrimitiveComponent * otherComp_, int32 otherBodyIndex_, bool bFromSweepO, const FHitResult & sweepResult_)
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
					widget->GetUserWidgetObject()->AddToViewport();
					//LoadText();
					ASRPGPlayerController* control = Cast<ASRPGPlayerController>(GetWorld()->GetFirstPlayerController());
					if (control)
					{
						control->SetInputMode(FInputModeUIOnly());
						if (hasUpdatedCentral == false)
						{
							control->FocusOnThisNPC(this, 0.45f);
						}
					}

				}

			}
		}
	}
}

void ABranchNPC::SetCentralNPC(ACentralNPC* ref_)
{
	central = ref_;
}

void ABranchNPC::SetChanceOfSuccessEffect(float value_)
{
	chanceOfSuccessEffect = value_;
}
