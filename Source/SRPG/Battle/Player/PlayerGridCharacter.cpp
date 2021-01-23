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
#include "../../ExternalFileReader/ExternalFileReader.h"
#include "TimerManager.h"
#include "../Grid/Obstacle.h"
#include "Kismet/GameplayStatics.h"
#include "../Weapons/WeaponBase.h"
#include "PCameraShake.h"


APlayerGridCharacter::APlayerGridCharacter() :AGridCharacter()
{
	overheadWidgetComp->AddRelativeRotation(FRotator(90.0f, -178.0f, 1.25f));
	fighterID = 0;
	chosenSkillAnimIndex = 0;
}

void APlayerGridCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(overheadWidgetComp)
		if(overheadWidgetComp->GetUserWidgetObject())
			if(!overheadWidgetComp->GetUserWidgetObject()->IsInViewport())
					overheadWidgetComp->GetUserWidgetObject()->AddToViewport();
}

void APlayerGridCharacter::StartPlayerTurn()
{
	UpdateOriginTile();
	bHasMoved = false;
	bHasDoneAnAction = false;
	currentState = EGridCharState::IDLE;

	statsComp->CheckStatBuffNerfStatus();
}

void APlayerGridCharacter::EndPlayerTurn()
{
	endText = "";
}

void APlayerGridCharacter::Selected()
{
	if (btlManager->GetPhase() == BTL_PLY) //Player phase
	{
		if (widgetComp)
			if(!widgetComp->GetUserWidgetObject()->IsInViewport())
				widgetComp->GetUserWidgetObject()->AddToViewport();
	}
}
void APlayerGridCharacter::NotSelected()
{
	if(widgetComp)
		if(widgetComp->GetUserWidgetObject()->IsInViewport())
			widgetComp->GetUserWidgetObject()->RemoveFromViewport();

	if (originTile)
		originTile->GetGridManager()->ClearHighlighted();

	if (currentState != EGridCharState::FINISHED)
			currentState = EGridCharState::IDLE;

	if (btlManager)
		btlManager->ActivateOutlines(false);
}
void APlayerGridCharacter::HighlightMovementPath()
{
	if (originTile)
	{
		currentState = EGridCharState::MOVING;

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
		//UE_LOG(LogTemp, Warning, TEXT("Highlighting attack row %d depth %d"), statsComp->GetStatValue(STAT_WRS), statsComp->GetStatValue(STAT_WDS));
		tile->GetGridManager()->UpdateCurrentTile(tile, statsComp->GetStatValue(STAT_WRS), statsComp->GetStatValue(STAT_WDS), TILE_ATK, statsComp->GetStatValue(STAT_PURE));
		if(btlCtrl)
			btlCtrl->SetHoverTargeting(true);
		if (btlManager)
			btlManager->ActivateOutlines(true);
	}
}


void APlayerGridCharacter::ActivateWeaponAttack()
{
	//Calculate hit and crit chances before applying damage
	int hitModifier = statsComp->GetStatValue(STAT_WHT) / 2; //Every 2 points in hit, gives us one point in HM
	int critModifier = 1; //Crit starts at 1 (not a critical attack)
	bool crit_ = false;
	if (actionTargets.Num() > 0)
	{
		if (FMath::RandRange(0, HIT_CRIT_BASE) + hitModifier >= static_cast<int>(actionTargets[0]->GetStat(STAT_HIT)))
		{
			if (statsComp->GetStatValue(STAT_CRT) >= FMath::RandRange(0, HIT_CRIT_BASE))
			{
				critModifier = 2;
				//Show crit
				crit_ = true;
			}

			//Spawn an emitter based on the weapon index
			if (btlManager)
				btlManager->SpawnWeaponEmitter(actionTargets[0]->GetActorLocation(), statsComp->GetStatValue(STAT_WPI));

			actionTargets[0]->GridCharTakeDamage(statsComp->GetStatValue(STAT_ATK) * critModifier, this, crit_, statsComp->GetStatValue(STAT_WPN_EFFECT));

			//Affect the crowd
			if (statsComp->AddTempCRD(CRD_ATK))
			{
				if (crdManager)
					crdManager->UpdateFavor(true);
			}

			//Add exp
			statsComp->AddToStat(STAT_EXP, ATK_RWRD);

			//Play camera shake
			if (btlCtrl && cameraShake)
				btlCtrl->PlayerCameraManager->PlayCameraShake(cameraShake, CAM_SHAKE_RATE);
		}
		else
		{
			//Show miss
			actionTargets[0]->GridCharReactToMiss();
		}
	}
	else if (targetObstacle) //If we don't have a character target, check if we're attacking an obstacle
	{
		//Check for critical
		if (statsComp->GetStatValue(STAT_CRT) >= FMath::RandRange(0, HIT_CRIT_BASE))
		{
			critModifier = 2;
		}

		//Spawn an emitter based on the weapon index
		if (btlManager)
			btlManager->SpawnWeaponEmitter(targetObstacle->GetActorLocation(), statsComp->GetStatValue(STAT_WPI));

		targetObstacle->ObstacleTakeDamage(statsComp->GetStatValue(STAT_ATK) * critModifier, statsComp->GetStatValue(STAT_WPN_EFFECT));
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

	if(actionTargets.Num()>0)
		actionTargets.Empty();
}


void APlayerGridCharacter::UpdateCharacterSkills()
{
	if (skills.Num() == 0) //No need to access the table every time we need to use a skill
	{
		if (fileReader)
		{
			TArray<FSkillTableStruct*> weaponSkills = fileReader->GetOffesniveSkills(0, statsComp->GetStatValue(STAT_WPI), statsComp->GetStatValue(STAT_WSN), statsComp->GetStatValue(STAT_WSI), statsComp->GetStatValue(STAT_LVL));
			TArray<FSkillTableStruct*> armorSkills = fileReader->GetDefensiveSkills(1, statsComp->GetStatValue(STAT_ARI), statsComp->GetStatValue(STAT_ASN), statsComp->GetStatValue(STAT_ASI), statsComp->GetStatValue(STAT_LVL));
			for (auto w : weaponSkills)
			{
				skills.Push(*w);
			}

			for (auto a : armorSkills)
			{
				skills.Push(*a);
			}

		}
	}
}

void APlayerGridCharacter::UseSkill(int index_)
{
	//check if we have enough pips first.
	if (index_ >= 0 && index_ < skills.Num())
	{
		if (skills[index_].pip <= statsComp->GetStatValue(STAT_PIP)) //Make sure we have enough pips
		{
			ATile* tile_ = GetMyTile();
			if (tile_)
			{
				int rowSpeed_ = skills[index_].rge;
				int depth_ = rowSpeed_ + 1;
				chosenSkill = skills[index_];
				chosenSkillAnimIndex = skills[index_].animationIndex;
				tile_->GetGridManager()->ClearHighlighted();
				tile_->GetGridManager()->UpdateCurrentTile(tile_, rowSpeed_, depth_, TILE_SKL, skills[index_].pure);
				currentState = EGridCharState::SKILLING;
				ABattleController* btlctrl = Cast< ABattleController>(GetWorld()->GetFirstPlayerController());
				if (btlctrl)
				{
					btlctrl->SetTargetingWithSkill(true, skills[index_].rows, skills[index_].depths, skills[index_].pure);
					btlCtrl->SetHoverTargeting(true);
				}

				if (btlManager)
					btlManager->ActivateOutlines(true);
			}
		}
		//TODO
		//If no pips are available, play an audio que
	}
}

void APlayerGridCharacter::ActivateSkillAttack()
{
	//Remove used pips
	statsComp->AddToStat(STAT_PIP,-chosenSkill.pip);
	//Ready all the variables
	float atkScaled = chosenSkill.atk * static_cast<float>(statsComp->GetStatValue(STAT_ATK));
	float intiScaled = chosenSkill.inti *static_cast<float>(statsComp->GetStatValue(STAT_INT));
	float skillValue = static_cast<float>(chosenSkill.value);

	if (skillValue < 0) //If this is a buff the skill value will be less than zero and the sacling varaibles need to match the sign of the value
	{
		atkScaled *= -1.0f;
		intiScaled *= -1.0f;
	}
	for (int i = 0; i < actionTargets.Num(); i++)
	{
		if (actionTargets[i])
		{
			if (chosenSkill.statIndex == STAT_HP && chosenSkill.value > 0) // If a skill does damage, we should check for hit and crit
			{
				//Calculate hit and crit chances
				int hitModifier = chosenSkill.hit / 2; //Every 2 points in hit, gives us one point in HM
				int critModifier = 1; //Crit starts at 1 (not a critical attack)

				//Check if we hit the action target
				if (FMath::RandRange(0, HIT_CRIT_BASE) + hitModifier >= static_cast<int>(actionTargets[i]->GetStat(STAT_HIT)))
				{
					bool crit_ = false;
					if (chosenSkill.crit >= FMath::RandRange(0, HIT_CRIT_BASE))
					{
						critModifier = 2;
						//Show crit
						crit_ = true;
					}

					//Affect the action target
					actionTargets[i]->GridCharReactToSkill((skillValue + atkScaled + intiScaled) * critModifier, chosenSkill.statIndex,
						chosenSkill.statusEffect, this, crit_);

					//Spawn The skill emitter
					if(chosenSkill.emitterIndex>=0 && chosenSkill.emitterIndex< onTargetskillEmitters.Num())
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), onTargetskillEmitters[chosenSkill.emitterIndex], actionTargets[i]->GetActorLocation(), FRotator::ZeroRotator);

					//Get CRD_SKL per target hit
					if (statsComp->AddTempCRD(CRD_SKL))
					{
						if (crdManager)
							crdManager->UpdateFavor(true);
					}

					//Play camera shake
					if (btlCtrl && cameraShake)
						btlCtrl->PlayerCameraManager->PlayCameraShake(cameraShake, CAM_SHAKE_RATE);
				}
				else
				{
					//Show miss
					actionTargets[i]->GridCharReactToMiss();
				}
			}
			else //If a skill buffs/nerfs/heals, it should always land
			{
				//Affect the action target
				actionTargets[i]->GridCharReactToSkill((skillValue + atkScaled + intiScaled), chosenSkill.statIndex,
					chosenSkill.statusEffect, this, false);

				//Spawn The skill emitter
				if (chosenSkill.emitterIndex >= 0 && chosenSkill.emitterIndex < onTargetskillEmitters.Num())
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), onTargetskillEmitters[chosenSkill.emitterIndex], actionTargets[i]->GetActorLocation(), FRotator::ZeroRotator);


				//Get CRD_SKL per target hit
				if (statsComp->AddTempCRD(CRD_SKL))
				{
					if (crdManager)
						crdManager->UpdateFavor(true);
				}
			}
		}
	}

	if (targetObstacle)
	{
		//Obstacles can only be affected by damage skills
		if (chosenSkill.statIndex == STAT_HP && chosenSkill.value > 0)
		{
			int critModifier = 1; //Crit starts at 1 (not a critical attack)
			//Check for critical
			if (chosenSkill.crit >= FMath::RandRange(0, HIT_CRIT_BASE))
			{
				critModifier = 2;
			}

			//Spawn an emitter based on the skill emitter index
			if (btlManager)
				btlManager->SpawnWeaponEmitter(targetObstacle->GetActorLocation(), chosenSkill.emitterIndex);

			targetObstacle->ObstacleTakeDamage((skillValue + atkScaled + intiScaled) * critModifier,chosenSkill.statusEffect);

		}
	}

	//You've used a skill, so get crd points regardless of the result
	if (statsComp->AddTempCRD(chosenSkill.fls))
	{
		if (crdManager)
			crdManager->UpdateFavor(true);
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
	if (actionTargets.Num() > 0)
	{
		statsComp->AddToStat(STAT_EXP, SKL_RWRD + chosenSkill.pip * 10); //Pip converted to exp. This way skills that cost a lot reward a lot
		actionTargets.Empty();
	}
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
		statsComp->PushAStat(0); //26
		statsComp->PushAStat(0); //27
		statsComp->PushAStat(0); //28
		statsComp->SetOwnerRef(this);
		AddEquipmentStats(2);
		UpdateCharacterSkills();
	}
}

void APlayerGridCharacter::SetFighterID(int fighterID_)
{
	fighterID = fighterID_;
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
	endText = "E";

	if (btlCtrl)
		btlCtrl->ResetControlledCharacter(); //Also calls NotSelected()
}

void APlayerGridCharacter::GridCharReatToElemental(float damage_, int statusEffectIndex_)
{
	if (damage_ > 0)
	{
		damage_ = damage_ - ((static_cast<float>(statsComp->GetStatValue(STAT_DEF)) / (damage_ + static_cast<float>(statsComp->GetStatValue(STAT_DEF)))) * damage_);
		animInstance->SetDamage(static_cast<int> (damage_));
		statsComp->AddToStat(STAT_HP, static_cast<int>(-damage_));


		if (statsComp->GetStatValue(STAT_HP) <= 1)
		{
			GetMyTile()->SetOccupied(false);
			for (int i = 0; i < TargetedByTheseCharacters.Num(); i++)
			{
				TargetedByTheseCharacters[i]->IamDeadStopTargetingMe();
			}
			Intermediate::GetInstance()->PushUnitToDead(fighterID); //Store the ID of the fighter
			if (statsComp->AddTempCRD(CRD_DED)) //You're dead so you also lose points
			{
				crdManager->UpdateFavor(false);
			}
			if (btlManager)
				btlManager->HandlePlayerDeath(this);
			if (animInstance)
				animInstance->DeathAnim();
		}

		for (int i = 0; i < equippedWeapons.Num(); i++)
		{
			if (equippedWeapons[i])
				equippedWeapons[i]->Destroy();
		}

		//TODO
		//Handle champion and villain situation
	}
	statsComp->CheckIfAffectedByStatusEffect(statusEffectIndex_);
}

void APlayerGridCharacter::GridCharTakeDamage(float damage_, AGridCharacter* attacker_, bool crit_, int statusEffect_)
{
	//Rotate to face attacker
	Super::GridCharTakeDamage(damage_, attacker_,crit_, statusEffect_);
	//update stats component
	damage_ = damage_ - ((static_cast<float>(statsComp->GetStatValue(STAT_DEF)) / (damage_ + static_cast<float>(statsComp->GetStatValue(STAT_DEF)))) * damage_);
	animInstance->SetDamage(static_cast<int> (damage_));
	statsComp->AddToStat(STAT_HP, static_cast<int>(-damage_));
	statsComp->CheckIfAffectedByStatusEffect(statusEffect_);
	//UE_LOG(LogTemp, Warning, TEXT("Actually  Health after taking damage %d"), statsComp->GetStatValue(STAT_HP));
	//Check if dead
	if (statsComp->GetStatValue(STAT_HP) <= 1)
	{
		GetMyTile()->SetOccupied(false);
		for (int i = 0; i < TargetedByTheseCharacters.Num(); i++)
		{
			TargetedByTheseCharacters[i]->IamDeadStopTargetingMe();
		}
		Intermediate::GetInstance()->PushUnitToDead(fighterID); //Store the ID of the fighter
		attacker_->YouHaveKilledYouTarget(true);
		if (statsComp->AddTempCRD(CRD_DED)) //You're dead so you also lose points
		{
			crdManager->UpdateFavor(false);
		}
		if (btlManager)
			btlManager->HandlePlayerDeath(this);
		if (animInstance)
			animInstance->DeathAnim();
		

		//Handle champion/villain death
		switch (championIndex)
		{
		case 0: //Champion dead
			crdManager->ChampVillainIsDead(true);
			attacker_->YouHaveJustKilledAChampion(0); //You've just killed a regular champion
			break;
		case 1: //Villain dead
			crdManager->ChampVillainIsDead(false);
			attacker_->YouHaveJustKilledAChampion(1); //You've just killed a villain
			break;
		case 2: //perma champion dead
			crdManager->SetPermaChampion(false);
			attacker_->YouHaveJustKilledAChampion(2); //You've just killed a perma champion
			break;
		}
		for (int i = 0; i < equippedWeapons.Num(); i++)
		{
			if (equippedWeapons[i])
				equippedWeapons[i]->Destroy();
		}
			
	}
}

void APlayerGridCharacter::GridCharReactToSkill(float damage_, int statIndex_, int statuEffectIndex_, AGridCharacter* attacker_, bool crit_)
{
	Super::GridCharReactToSkill(damage_, statIndex_, statuEffectIndex_, attacker_, crit_);

	statsComp->CheckIfAffectedByStatusEffect(statuEffectIndex_);
	if (statIndex_ == STAT_HP && damage_ > 0) //If the skill does HP damage
	{
		//update stats component
		damage_ = damage_ - ((static_cast<float>(statsComp->GetStatValue(STAT_DEF)) / (damage_ + static_cast<float>(statsComp->GetStatValue(STAT_DEF)))) * damage_);
		if (animInstance)
		{
			animInstance->GotHit(crit_);
			animInstance->SetDamage(static_cast<int> (damage_));
		}

		statsComp->AddToStat(STAT_HP, static_cast<int>(-damage_));
		//Check if dead
		if (statsComp->GetStatValue(STAT_HP) <= 1)
		{
			GetMyTile()->SetOccupied(false);
			for (int i = 0; i < TargetedByTheseCharacters.Num(); i++)
			{
				if(TargetedByTheseCharacters[i])
					TargetedByTheseCharacters[i]->IamDeadStopTargetingMe();
			}
			Intermediate::GetInstance()->PushUnitToDead(fighterID);
			attacker_->YouHaveKilledYouTarget(true);
			if (statsComp->AddTempCRD(CRD_DED)) //You're dead so you also lose favor
			{
				crdManager->UpdateFavor(false);
			}


			if (btlManager)
				btlManager->HandlePlayerDeath(this);
			if (animInstance)
				animInstance->DeathAnim();


			//Handle champion/villain death
			switch (championIndex)
			{
			case 0: //Champion dead
				crdManager->ChampVillainIsDead(true);
				attacker_->YouHaveJustKilledAChampion(0); //You've just killed a regular champion
				break;
			case 1: //Villain dead
				crdManager->ChampVillainIsDead(false);
				attacker_->YouHaveJustKilledAChampion(1); //You've just killed a villain
				break;
			case 2: //perma champion dead
				crdManager->SetPermaChampion(false);
				attacker_->YouHaveJustKilledAChampion(2); //You've just killed a perma champion
				break;
			}

			for (int i = 0; i < equippedWeapons.Num(); i++)
			{
				if (equippedWeapons[i])
					equippedWeapons[i]->Destroy();
			}
		}
	}
	else if((statIndex_ == STAT_HP || statIndex_ == STAT_PIP) && damage_ < 0) //Healing skill
	{
		animInstance->ChangeStats(true);
		statsComp->AddToStat(statIndex_, static_cast<int>(-damage_));
	}
	else //Buff or nerf to a stat
	{
		if(damage_ < 0) //Buff (I know it's confusing)
			animInstance->ChangeStats(true);
		else
			animInstance->ChangeStats(false);

		statsComp->AddTempToStat(statIndex_, static_cast<int>(-damage_));
	}
}

void APlayerGridCharacter::UpdateCurrentEXP()
{
	if (statsComp)
	{
		//Revert the buffs and nerfs the character received due to being a champion or a villain
		if (championBuffCount > 0)
		{
			if (championIndex == 0 || championIndex == 2) //if I'm a champion
			{
				for (int i = 0; i < championBuffCount; i++)
				{
					statsComp->RevertChampionVillainStatsUpdate(true);
				}
			}
			else if (championIndex == 1) //if I'm a villain
			{
				for (int i = 0; i < championBuffCount; i++)
				{
					statsComp->RevertChampionVillainStatsUpdate(false);
				}
			}
		}
		//Check for level up
		statsComp->CheckLevelUp(false);
	}
}

void APlayerGridCharacter::CheckChangeStats()
{
	int changedStat = Intermediate::GetInstance()->GetStatsChange(CHG_STAT_PLY);
	if (changedStat != -1)
	{
		statsComp->AddToStat(changedStat, static_cast<float>(statsComp->GetStatValue(changedStat))*PLY_IMP_STAT);
		animInstance->ChangeStats(true);
	}
}

ABattleManager* APlayerGridCharacter::GetBattleManager()
{
	return btlManager;
}