// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGridCharacter.h"
#include "../Grid/Tile.h"
#include "../Grid/GridManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "../Grid/Tile.h"
#include "../PathComponent.h"
#include "../BattleManager.h"
#include "Components/WidgetComponent.h"


APlayerGridCharacter::APlayerGridCharacter() :AGridCharacter()
{
	attackRowSpeed = 2;
	attackDepth = 2;
}
void APlayerGridCharacter::Selected()
{
	if (btlManager->GetPhase() == 1) //Player phase
	{
		if (widgetComp)
			widgetComp->GetUserWidgetObject()->AddToViewport();
	}
}
void APlayerGridCharacter::NotSelected()
{
	if (widgetComp)
		widgetComp->GetUserWidgetObject()->RemoveFromViewport();

	if (originTile)
		originTile->GetGridManager()->ClearHighlighted();

	currentState = EGridCharState::IDLE;
}
void APlayerGridCharacter::HighlightMovementPath()
{
	if (originTile)
	{
		currentState = EGridCharState::IDLE;

		originTile->GetGridManager()->ClearHighlighted();

		if (movementPath.Num() > 0)
			movementPath.Empty();
		if (pathComp)
			originTile->GetGridManager()->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(),0);
	}
}
void APlayerGridCharacter::HighlightRegularAttackPath()
{
	ATile* tile = GetMyTile();
	if (tile)
	{
		currentState = EGridCharState::ATTACKING;
		tile->GetGridManager()->ClearHighlighted();
		tile->GetGridManager()->UpdateCurrentTile(tile, attackRowSpeed, attackDepth, 1);
	}
}


void APlayerGridCharacter::ActivateWeaponAttack()
{
	//TODO
	//Calculate hit and crit chances before applying damage
	//Get the damage from the equipment
	//Affect the crowd
	if (actionTarget)
	{
		actionTarget->GridCharTakeDamage(1.0f, this);
	}
}

void APlayerGridCharacter::ActivateSkillAttack()
{
	//TODO 
	//Remove used pips
	//Calculate hit and crit chances
	//Tell the battlemanager to spawn the emitter on the action target
	//Affect the crowd
	if (actionTarget)
	{
		actionTarget->GridCharReactToSkill(skills[chosenSkill].value, skills[chosenSkill].statIndex, 
										   skills[chosenSkill].statusEffect,this);
	}
}