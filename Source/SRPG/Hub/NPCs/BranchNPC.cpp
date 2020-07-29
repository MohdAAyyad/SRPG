// Fill out your copyright notice in the Description page of Project Settings.


#include "BranchNPC.h"
#include "Hub/NPCs/CentralNPC.h"
#include "Components/WidgetComponent.h"

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
	if (hasUpdatedCentral == false)
	{
		line = "I'm sure if you were to talk to the guy over there, things would go your way.";
		central->UpdateChanceOfSuccess(chanceOfSuccessEffect);
		hasUpdatedCentral = true;
	}
	else
	{
		line = "Listen pal, I've told you all I know";
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
