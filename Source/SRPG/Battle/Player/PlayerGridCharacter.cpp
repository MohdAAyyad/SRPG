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
#include "../BattleController.h"
#include "Components/WidgetComponent.h"
#include "Animation/GridCharacterAnimInstance.h"
#include "TimerManager.h"



APlayerGridCharacter::APlayerGridCharacter() :AGridCharacter()
{
	overheadWidgetComp->AddRelativeRotation(FRotator(90.0f, -178.0f, 1.25f));
}

void APlayerGridCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerGridCharacter::StartPlayerTurn()
{
	UpdateOriginTile();
	bHasMoved = false;
	bHasDoneAnAction = false;
	currentState = EGridCharState::IDLE;
}

void APlayerGridCharacter::EndPlayerTurn()
{
	overheadWidgetComp->SetVisibility(false);
}

void APlayerGridCharacter::Selected()
{
	if (btlManager->GetPhase() == BTL_PLY) //Player phase
	{
		if (currentState != EGridCharState::FINISHED)
		{
			if (widgetComp)
				widgetComp->GetUserWidgetObject()->AddToViewport();
		}
	}
}
void APlayerGridCharacter::NotSelected()
{
	if(widgetComp->GetUserWidgetObject()->IsInViewport())
		widgetComp->GetUserWidgetObject()->RemoveFromViewport();

	if (originTile)
		originTile->GetGridManager()->ClearHighlighted();

	if (currentState != EGridCharState::FINISHED)
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
		UE_LOG(LogTemp, Warning, TEXT("Highlighting attack row %d depth %d"), statsComp->GetStatValue(STAT_WRS), statsComp->GetStatValue(STAT_WDS));
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
		actionTargets[0]->GridCharTakeDamage(statsComp->GetStatValue(STAT_ATK) * 10, this);
		
		if (statsComp->AddTempCRD(CRD_ATK))
		{
			if (crdManager)
				crdManager->UpdateFavor(true);
		}

		statsComp->AddToStat(STAT_EXP, 20);
	}

	if (bHasMoved && bHasDoneAnAction) //Has moved and has done an action, we're done
	{
		FinishState();
	}
	else
	{
		NotSelected();
		Selected();
	}

	actionTargets.Empty();
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

	if (bHasMoved && bHasDoneAnAction) //Has moved and has done an action, we're done
	{
		FinishState();
	}
	else
	{
		NotSelected();
		Selected();
	}

	actionTargets.Empty();
}

void APlayerGridCharacter::SetFighterIndex(int index_)
{
	fighterIndex = index_;

	if (statsComp)
	{
		TArray<FFighterTableStruct>selectedFighters = Intermediate::GetInstance()->GetSelectedFighters();
		statsComp->PushAStat(selectedFighters[fighterIndex].hp); //0
		statsComp->PushAStat(selectedFighters[fighterIndex].pip); //1
		statsComp->PushAStat(selectedFighters[fighterIndex].atk); //2
		statsComp->PushAStat(selectedFighters[fighterIndex].def); //3
		statsComp->PushAStat(selectedFighters[fighterIndex].intl); //4
		statsComp->PushAStat(selectedFighters[fighterIndex].spd); //5		
		statsComp->PushAStat(selectedFighters[fighterIndex].crit); //6
		statsComp->PushAStat(selectedFighters[fighterIndex].agl); //7
		statsComp->PushAStat(selectedFighters[fighterIndex].crd); //8
		statsComp->PushAStat(selectedFighters[fighterIndex].level); //9
		statsComp->PushAStat(selectedFighters[fighterIndex].currentEXP); //10
		statsComp->PushAStat(selectedFighters[fighterIndex].neededEXPToLevelUp); //11
		statsComp->PushAStat(selectedFighters[fighterIndex].weaponIndex); //12
		statsComp->PushAStat(selectedFighters[fighterIndex].armorIndex); //13
		statsComp->PushAStat(selectedFighters[fighterIndex].equippedWeapon); //14
		statsComp->PushAStat(selectedFighters[fighterIndex].equippedArmor); //15
		statsComp->PushAStat(selectedFighters[fighterIndex].equippedAccessory); //16
		statsComp->PushAStat(0); //17
		statsComp->PushAStat(0); //18
 		statsComp->PushAStat(0); //19
		statsComp->PushAStat(0); //20
		statsComp->PushAStat(0); //21
		statsComp->PushAStat(0); //22
		statsComp->PushAStat(0); //23
		statsComp->PushAStat(0); //24
		statsComp->PushAStat(selectedFighters[fighterIndex].archetype); //25
		AddEquipmentStats(2);
	}
}

void APlayerGridCharacter::RemoveWidgetFromVP()
{
	if (widgetComp)
	{
		if (widgetComp->GetUserWidgetObject()->IsInViewport())
			widgetComp->GetUserWidgetObject()->RemoveFromViewport();
	}
}

void APlayerGridCharacter::FinishState()
{
	currentState = EGridCharState::FINISHED;
	overheadWidgetComp->SetVisibility(true);

	if (btlCtrl)
		btlCtrl->ResetControlledCharacter(); //Also calls NotSelected()
}

void APlayerGridCharacter::GridCharTakeDamage(float damage_, AGridCharacter* attacker_)
{
	//Rotate to face attacker
	Super::GridCharTakeDamage(damage_, attacker_);
	//update stats component
	statsComp->AddToStat(STAT_HP, -damage_);
	//UE_LOG(LogTemp, Warning, TEXT("Actually  Health after taking damage %d"), statsComp->GetStatValue(STAT_HP));
	//Check if dead
	if (statsComp->GetStatValue(STAT_HP) <= 1)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Die player die"));
		if (btlManager)
			btlManager->HandlePlayerDeath(this);
		if (animInstance)
			animInstance->DeathAnim();
	}
}

void APlayerGridCharacter::UpdateCurrentEXP()
{
	if (statsComp)
	{
		statsComp->CheckLevelUp(false);
	}
}