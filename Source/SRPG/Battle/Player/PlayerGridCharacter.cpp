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
#include "Definitions.h"
#include "../Crowd/BattleCrowd.h"
#include "../StatsComponent.h"
#include "Intermediary/Intermediate.h"


APlayerGridCharacter::APlayerGridCharacter() :AGridCharacter()
{
}

void APlayerGridCharacter::BeginPlay()
{
	Super::BeginPlay();
}
void APlayerGridCharacter::Selected()
{
	if (btlManager->GetPhase() == BTL_PLY) //Player phase
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
			originTile->GetGridManager()->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(),TILE_MOV, 0);
	}
}
void APlayerGridCharacter::HighlightRegularAttackPath()
{
	ATile* tile = GetMyTile();
	if (tile && statsComp)
	{
		currentState = EGridCharState::ATTACKING;
		tile->GetGridManager()->ClearHighlighted();
		tile->GetGridManager()->UpdateCurrentTile(tile, statsComp->GetStatValue(STAT_WRS), statsComp->GetStatValue(STAT_WDS), TILE_ATK, statsComp->GetStatValue(STAT_PURE));
	}
}


void APlayerGridCharacter::ActivateWeaponAttack()
{
	//TODO
	//Calculate hit and crit chances before applying damage
	//Get the damage from the equipment
	//Affect the crowd
	if (actionTargets[0])
	{
		actionTargets[0]->GridCharTakeDamage(1.0f, this);
		
		if (statsComp->AddTempCRD(CRD_ATK))
		{
			if (crdManager)
				crdManager->UpdateFavor(true);
		}
	}
}

void APlayerGridCharacter::ActivateSkillAttack()
{
	//TODO 
	//Remove used pips
	//Calculate hit and crit chances
	//Tell the battlemanager to spawn the emitter on the action target
	//Affect the crowd
	for (int i = 0; i < actionTargets.Num(); i++)
	{
		if (actionTargets[i])
		{
			actionTargets[i]->GridCharReactToSkill(skills[chosenSkill].value, skills[chosenSkill].statIndex,
				skills[chosenSkill].statusEffect, this);

			if (statsComp->AddTempCRD(skills[chosenSkill].fls))
			{
				if (crdManager)
					crdManager->UpdateFavor(true);
			}
		}
	}

	actionTargets.Empty();
}

void APlayerGridCharacter::SetFighterIndex(int index_)
{
	fighterIndex = index_;

	if (statsComp)
	{
		TArray<FFighterTableStruct>selectedFighters = Intermediate::GetInstance()->GetSelectedFighters();
		statsComp->PushAStat(selectedFighters[fighterIndex].hp);
		statsComp->PushAStat(selectedFighters[fighterIndex].pip);
		statsComp->PushAStat(selectedFighters[fighterIndex].atk);
		statsComp->PushAStat(selectedFighters[fighterIndex].def);
		statsComp->PushAStat(selectedFighters[fighterIndex].intl);
		statsComp->PushAStat(selectedFighters[fighterIndex].spd);		
		statsComp->PushAStat(selectedFighters[fighterIndex].crit);
		statsComp->PushAStat(selectedFighters[fighterIndex].agl);
		statsComp->PushAStat(selectedFighters[fighterIndex].crd);
		statsComp->PushAStat(selectedFighters[fighterIndex].level);
		statsComp->PushAStat(selectedFighters[fighterIndex].currentEXP);
		statsComp->PushAStat(selectedFighters[fighterIndex].neededEXPToLevelUp);
		statsComp->PushAStat(selectedFighters[fighterIndex].weaponIndex);
		statsComp->PushAStat(selectedFighters[fighterIndex].armorIndex);
		statsComp->PushAStat(selectedFighters[fighterIndex].equippedWeapon);
		statsComp->PushAStat(selectedFighters[fighterIndex].equippedArmor);
		statsComp->PushAStat(selectedFighters[fighterIndex].equippedAccessory);
		statsComp->PushAStat(0);
		statsComp->PushAStat(0);
		statsComp->PushAStat(0);
		statsComp->PushAStat(0);
		statsComp->PushAStat(0);
		statsComp->PushAStat(0);
		statsComp->PushAStat(0);
		statsComp->PushAStat(selectedFighters[fighterIndex].archetype);
		AddEquipmentStats(2);
	}
}