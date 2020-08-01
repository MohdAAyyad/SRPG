// Fill out your copyright notice in the Description page of Project Settings.


#include "BranchNPC.h"
#include "Hub/NPCs/CentralNPC.h"
#include "Components/WidgetComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"

void ABranchNPC::EndDialogue()
{
	UE_LOG(LogTemp, Warning, TEXT("UnInteracted!"));
	if (widget)
	{
		widget->GetUserWidgetObject()->RemoveFromViewport();
	}
}

void ABranchNPC::LoadText()
{
	if (hasUpdatedCentral == false && central && chanceOfSuccessEffect >= 0)
	{
		//line = "I'm sure if you were to talk to the guy over there, things could go your way.";
		FActivityDialogueTableStruct row = fileReader->GetPositiveBranch(central->GetCentralActivityIndex(), 0);
		line = row.dialogue;
		//line = filedea
		central->UpdateChanceOfSuccess(chanceOfSuccessEffect);
		hasUpdatedCentral = true;
	}
	else if (hasUpdatedCentral == false && central && chanceOfSuccessEffect < 0)
	{
		FActivityDialogueTableStruct row = fileReader->GetNegativeBranch(central->GetCentralActivityIndex(), 0);
		line = row.dialogue;
		central->UpdateChanceOfSuccess(chanceOfSuccessEffect);
		hasUpdatedCentral = true;
	}
	else
	{
		line = "Listen pal, I've told you all I know";
	}
}

void ABranchNPC::BeginPlay()
{
	Super::BeginPlay();
	chanceOfSuccessEffect = FMath::RandRange(-35, 35);
	hasUpdatedCentral = false;
}

void ABranchNPC::SetCentralNPC(ACentralNPC* ref_)
{
	central = ref_;
}

void ABranchNPC::SetChanceOfSuccessEffect(float value_)
{
	chanceOfSuccessEffect = value_;
}
