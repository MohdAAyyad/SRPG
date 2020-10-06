// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBaseGridCharacter.h"
#include "AIManager.h"
#include "../Player/PlayerGridCharacter.h"
#include "../Grid/Tile.h"
#include "../Grid/GridManager.h"
#include "../PathComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "../BattleController.h"
#include "Definitions.h"
#include "../Crowd/BattleCrowd.h"
#include "../BattleManager.h"
#include "../Crowd/CrowdItem.h"
#include "Components/CapsuleComponent.h"
#include "Definitions.h"
#include "../StatsComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Intermediary/Intermediate.h"
#include "DecisionComp.h"
#include "Components/WidgetComponent.h"
#include "Animation/GridCharacterAnimInstance.h"


AEnemyBaseGridCharacter::AEnemyBaseGridCharacter() :AGridCharacter()
{
	detectionRadius = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionRadius"));
	detectionRadius->SetupAttachment(RootComponent);
	detectionRadius->SetGenerateOverlapEvents(true);
	detectionRadius->SetCollisionProfileName("EnemyDetectionRadius");
	detectionRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	decisionComp = CreateDefaultSubobject<UDecisionComp>(TEXT("Decision Comp"));

	targetCharacter = nullptr;
	targetTile = nullptr;
	targetItem = nullptr;
	bWillMoveAgain = true;
	bCannotFindTile = false;
	weaponIndex = 0;
	armorIndex = 0;
	bHealer = false; //Changed in the BP editor. Some enemies are just destined to be healers.
}

void AEnemyBaseGridCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOriginTile();
	if (detectionRadius)
		detectionRadius->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBaseGridCharacter::DetectItem);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBaseGridCharacter::TakeItem);

}

void AEnemyBaseGridCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCannotFindTile) //Necessary in case the enemy needed a new destiantion while transitioning between tiles
	{
		ATile* myTile_ = GetMyTile();

		if (myTile_)
		{
			bCannotFindTile = false;
			bMoving = false;
			MoveCloserToTargetPlayer(myTile_);
		}
	}
}
void AEnemyBaseGridCharacter::SetManagers(AAIManager* ref_, AGridManager* gref_, ABattleManager* bref_, ABattleCrowd* cref_)
{
	//Called by AI Manager on Beginplay
	aiManager = ref_;
	gridManager = gref_;
	btlManager = bref_;
	crdManager = cref_;

	statsComp->InitStatsAtZero();
	statsComp->AddToStat(STAT_WPI, weaponIndex);
	statsComp->AddToStat(STAT_ARI, armorIndex);
	statsComp->ScaleLevelWithArchetype(Intermediate::GetInstance()->GetNextOpponent().level, Intermediate::GetInstance()->GetNextOpponent().archtype);
	AddEquipmentStats(2);

	decisionComp->SetRefs(aiManager, btlManager, this);
	decisionComp->UpdatePattrn(statsComp->GetStatValue(STAT_LVL),bHealer);
}

void AEnemyBaseGridCharacter::AddEquipmentStats(int tableIndex_)
{

	//Get the best equipment you can equip from the table

	FEquipmentTableStruct weapon;
	FEquipmentTableStruct armor;
	FEquipmentTableStruct accessory;

	//Get the stats of the equipment and add them to the character's stats
	if (fileReader)
	{
		weapon = fileReader->GetEquipmentByLevel(tableIndex_, statsComp->GetStatValue(STAT_LVL),EQU_WPN, statsComp->GetStatValue(STAT_WPI));
		armor = fileReader->GetEquipmentByLevel(tableIndex_, statsComp->GetStatValue(STAT_LVL),EQU_ARM, statsComp->GetStatValue(STAT_ARI));
		accessory = fileReader->GetEquipmentByLevel(tableIndex_, statsComp->GetStatValue(STAT_LVL),EQU_ACC,-1);
		

		statsComp->UpdateMaxHpAndMaxPip(weapon.hp + armor.hp + accessory.hp, weapon.pip + armor.pip + accessory.pip);
		statsComp->AddToStat(STAT_HP, weapon.hp + armor.hp + accessory.hp);
		statsComp->AddToStat(STAT_PIP, weapon.pip + armor.pip + accessory.pip);
		statsComp->AddToStat(STAT_ATK, weapon.atk + armor.atk + accessory.atk);
		statsComp->AddToStat(STAT_DEF, weapon.def + armor.def + accessory.def);
		statsComp->AddToStat(STAT_INT, weapon.intl + armor.intl + accessory.intl);
		statsComp->AddToStat(STAT_SPD, weapon.spd + armor.spd + accessory.spd);
		statsComp->AddToStat(STAT_CRT, weapon.crit + armor.crit + accessory.crit);
		statsComp->AddToStat(STAT_HIT, armor.hit + accessory.hit); //Agility is not affected by the weapon
		statsComp->AddToStat(STAT_WHT, weapon.hit); //Weapon hit is affected by the weapon's hit
		statsComp->AddToStat(STAT_CRD, weapon.crd + armor.crd + accessory.crd);
		statsComp->AddToStat(STAT_WSI, weapon.skillsIndex);
		statsComp->AddToStat(STAT_WSN, weapon.skillsN);
		statsComp->AddToStat(STAT_ASI, armor.skillsIndex);
		statsComp->AddToStat(STAT_ASN, armor.skillsN);
		statsComp->AddToStat(STAT_WRS, weapon.range);
		statsComp->AddToStat(STAT_WDS, weapon.range + 1);
		statsComp->AddToStat(STAT_PURE, weapon.pure);
	}

	if (pathComp)
		pathComp->UpdateSpeed(statsComp->GetStatValue(STAT_SPD));
}

void AEnemyBaseGridCharacter::MoveCloserToTargetPlayer(ATile* startingTile_)
{
	ATile* myTile_ = startingTile_;
	if (!myTile_)
	{
		bCannotFindTile = true;//Only calculate the path if we have our current tile, otherwise, find your tile
	}
	else
	{
		bWillMoveAgain = false; //Move closer to player is always the last one to be called in terms of movement
		targetCharacter = decisionComp->FindTheOptimalTargetCharacter();
		if (targetCharacter)
		{
			targetTile = decisionComp->FindOptimalTargetTile(myTile_);
			
			MoveToTheTileWithinRangeOfThisTile(myTile_, targetTile);
		}
		else //If we couldn't find a character, don't attempt to find a tile. This will only happen whne all potential targets are dead. This check prevents an infinite loop of looking for a target
		{
			myTile_->SetOccupied(true);
			aiManager->FinishedMoving();
		}

	}
}
void AEnemyBaseGridCharacter::StartEnemyTurn()
{
	bWillMoveAgain = true; //Ready to obtain a new item
	targetItem = nullptr;
	CheckIfWeHaveAnyTargetItems();

}

void AEnemyBaseGridCharacter::Selected()
{
	UpdateOriginTile();
	if (gridManager && pathComp && originTile)
	{
		//Show the enemy's movement and attack ranges
		gridManager->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(), TILE_ENMH,0);
		gridManager->UpdateCurrentTile(originTile, statsComp->GetStatValue(STAT_WRS), statsComp->GetStatValue(STAT_WDS), TILE_ATK, statsComp->GetStatValue(STAT_PURE));
	}

	if (widgetComp)
	{
		if(!widgetComp->GetUserWidgetObject()->IsInViewport())
			widgetComp->GetUserWidgetObject()->AddToViewport();
	}
}

void AEnemyBaseGridCharacter::NotSelected()
{
	if (gridManager)
		gridManager->ClearHighlighted();

	if (widgetComp)
	{
		if (widgetComp->GetUserWidgetObject()->IsInViewport())
			widgetComp->GetUserWidgetObject()->RemoveFromViewport();
	}
}

void AEnemyBaseGridCharacter::ExecuteChosenAction()
{
	if (gridManager)
	{
		if (targetCharacter)
		{
			ATile* myTile_ = GetMyTile();
			if (myTile_)
			{
				if (decisionComp)
				{
					if (decisionComp->GetWillUseSkill())
					{
						chosenSkill = decisionComp->GetChosenSkill();
						gridManager->ClearHighlighted();
						gridManager->UpdateCurrentTile(myTile_, chosenSkill.rge, chosenSkill.rge +1, TILE_ENM, chosenSkill.pure);

						if (targetCharacter->GetMyTile()->GetHighlighted() == TILE_ENM) //Is the target within the skill's range?
						{
							btlCtrl->FocusOnGridCharacter(this, btlCtrl->focusRate);
							TArray<AGridCharacter*> targets_;
							targets_.Reserve(1);
							targets_.Push(targetCharacter);
							chosenSkillAnimIndex = chosenSkill.animationIndex;
							AttackUsingSkill(targets_, btlCtrl->focusRate);
						}
						else
						{
							if (aiManager)
								aiManager->FinishedAttacking();

							if (crdItems.Num() > 0) //Look for new items on your next turn as someone may take your item on their turn
								crdItems.Empty();
						}
					}
					else
					{
						//Regular attack
						gridManager->ClearHighlighted();
						gridManager->UpdateCurrentTile(myTile_, statsComp->GetStatValue(STAT_WRS), statsComp->GetStatValue(STAT_WDS), TILE_ENM, statsComp->GetStatValue(STAT_PURE));
						if (targetCharacter)
						{
							if (targetCharacter->GetMyTile()->GetHighlighted() == TILE_ENM) //Is the player within attack range?
							{
								btlCtrl->FocusOnGridCharacter(this, btlCtrl->focusRate);
								AttackUsingWeapon(targetCharacter, btlCtrl->focusRate);
							}
							else
							{
								if (aiManager)
									aiManager->FinishedAttacking();

								if (crdItems.Num() > 0) //Look for new items on your next turn as someone may take your item on their turn
									crdItems.Empty();
							}
						}
					}
				}
				
			}
			else //Could not find the tile, don't break, finish the turn instead
			{
				if (aiManager)
					aiManager->FinishedAttacking();
				if (crdItems.Num() > 0)
					crdItems.Empty();
			}
		}
		else //If you don't have a target yet, finish the attacking phase
		{
			if (aiManager)
				aiManager->FinishedAttacking();
			if (crdItems.Num() > 0)
				crdItems.Empty();
		}
	}
}

void AEnemyBaseGridCharacter::ActivateWeaponAttack()
{
	//TODO
	//Calculate hit and crit chances before applying damage
	int hitModifier = statsComp->GetStatValue(STAT_WHT) / 2; //Every 2 points in hit, gives us one point in HM
	int critModifier = 1; //Crit starts at 1 (not a critical attack)
	bool crit_ = false;
	if (actionTargets[0])
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

			//Damage the target
			actionTargets[0]->GridCharTakeDamage(statsComp->GetStatValue(STAT_ATK) *critModifier, this, crit_);

			//Affect the crowd
			if (statsComp->AddTempCRD(CRD_ATK))
			{
				if (crdManager)
					crdManager->UpdateFavor(false);
			}
		}
		else
		{
			//Show miss
			actionTargets[0]->GridCharReactToMiss();
		}
	}
	if (gridManager)
		gridManager->ClearHighlighted(); //Clear the highlighted tiles after completing the attack

	if (actionTargets.Num() > 0)
		actionTargets.Empty();
}

void AEnemyBaseGridCharacter::ActivateSkillAttack()
{
	//Remove used pips
	statsComp->AddToStat(STAT_PIP, -chosenSkill.pip);
	//Calculate hit and crit chances
	int hitModifier = chosenSkill.hit / 2; //Every 2 points in hit, gives us one point in HM
	int critModifier = 1; //Crit starts at 1 (not a critical attack)
	//Ready all the variables
	float atkScaled = chosenSkill.atk * static_cast<float>(statsComp->GetStatValue(STAT_ATK));
	float intiScaled = chosenSkill.inti *static_cast<float>(statsComp->GetStatValue(STAT_INT));
	float skillValue = static_cast<float>(chosenSkill.value);
	//Affect the crowd
	for (int i = 0; i < actionTargets.Num(); i++)
	{
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

			if (actionTargets[i])
			{
				//Affect the action target
				actionTargets[i]->GridCharReactToSkill((skillValue + atkScaled + intiScaled) * critModifier, chosenSkill.statIndex,
					chosenSkill.statusEffect, this, crit_);

				//Tell the battlemanager to spawn the emitter on the action target
				if (btlManager)
					btlManager->SpawnSkillEmitter(actionTargets[i]->GetActorLocation(), chosenSkill.emitterIndex);


				if (statsComp->AddTempCRD(CRD_SKL))
				{
					if (crdManager)
						crdManager->UpdateFavor(false);
				}
			}
		}
		else
		{
			//Show miss
			actionTargets[i]->GridCharReactToMiss();
		}
	}

	//You've used a skill, so get crd points regardless of the result
	if (statsComp->AddTempCRD(chosenSkill.fls))
	{
		if (crdManager)
			crdManager->UpdateFavor(true);
	}

	if(actionTargets.Num() > 0)
		actionTargets.Empty();
}

void AEnemyBaseGridCharacter::ResetCameraFocus()
{
	Super::ResetCameraFocus();
	FTimerHandle finishedAttackingHandle;
	//This way we allow the camera to defocus from one enemy before focusing on the next
	GetWorld()->GetTimerManager().SetTimer(finishedAttackingHandle, aiManager, &AAIManager::FinishedAttacking, btlCtrl->focusRate + 0.2f, false);

}

void AEnemyBaseGridCharacter::MoveAccordingToPath()
{
	if (movementPath.Num() > 0)
	{
		FVector direction = movementPath[movementPath.Num() - 1] - GetActorLocation();
		if (FMath::Abs(movementPath[movementPath.Num() - 1].X - GetActorLocation().X) > 10.0f || FMath::Abs(movementPath[movementPath.Num() - 1].Y - GetActorLocation().Y) > 10.0f)
		{
			AddMovementInput(direction, 1.0f);
		}
		else
		{
			movementPath.RemoveAt(movementPath.Num() - 1);
			if (movementPath.Num() == 0)
			{
				bMoving = false;
				if (!bWillMoveAgain) //If we have obtained an item, we're still gonna call pick movement destination so don't finish moving just yet
					// Will be false, once pickMovementDestination is called. If we have an  item pickMovementDestination will be called after colliding with the item
				{
					if (aiManager)
					{
						aiManager->FinishedMoving();
					}
				}
			}
		}
	}
}
void AEnemyBaseGridCharacter::MoveToTheTileWithinRangeOfThisTile(ATile* startingTile_, ATile* targetTile_)
{
	if (targetTile_)
	{
		if(!targetItem)
			gridManager->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(), TILE_ENM, 0);


		if (movementPath.Num() > 0)
			movementPath.Empty();

		if (!startingTile_) //No need to turn bCannotFindTile to true as startingTile_ will be a nullptr only when trying to get an item which always happens at the beginning of the turn which means the enemy is still standing.
			startingTile_ = GetMyTile();

		pathComp->SetCurrentTile(startingTile_);
		pathComp->SetTargetTile(targetTile_);
		pathComp->GetPath(); //Get the whole path towards the target
		TArray<ATile*> tPath = pathComp->GetMovementPath();

		TArray<int> tileVecIndexes;
		//We only care about the tiles within movement range
		for (int i = 0; i < tPath.Num(); i++)
		{
			if (tPath[i]->GetHighlighted() == TILE_ENM)
			{
				tileVecIndexes.Push(i);
			}
		}

		for (int j = 0; j < tileVecIndexes.Num(); j++)
		{
			movementPath.Push(tPath[tileVecIndexes[j]]->GetActorLocation());
		}

		//Make sure the last tile we can reach is not occupied. 
		//Sets it to occupied as well before actually moving to avoid two enemies deciding to go for the same tile
		if (tPath.Num() > 0 && movementPath.Num() > 0 && tileVecIndexes.Num()>0 && pathComp)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Adjusting path"));
			pathComp->AdjustPath(tPath[tileVecIndexes[0]], movementPath);
		}

		if (movementPath.Num() > 0)
		{
			bMoving = true;
		}
		else
		{
			if (aiManager)
				aiManager->FinishedMoving();
		}

	}
	
}

void AEnemyBaseGridCharacter::CheckIfWeHaveAnyTargetItems()
{
	UpdateOriginTile();

	if (crdItems.Num() > 0)
	{
		if (gridManager)
		{
			gridManager->ClearHighlighted();
			gridManager->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(), TILE_ENM,0);

			for (int i = 0; i < crdItems.Num(); i++)
			{
				if (crdItems[i])
				{
					//TODO
					//Check if we need the item first before marking it
					
					//If we have detected an item, make sure it's within movement range before marking it
					ATile* tile_ = crdItems[i]->GetMyTile();
					if (tile_)
					{
						if (tile_->GetHighlighted() == TILE_ENM)
						{
							if (crdItems[i]->MarkItem(this)) //True, means we've marked this items as our target and no other enemy should go for it
							{
								targetItem = crdItems[i];
								break;
							}
						}
					}

				}
			}
		}
	}		

	if (targetItem) //If we have a target item, then go there
	{
		MoveToTheTileWithinRangeOfThisTile(nullptr, targetItem->GetMyTile());
	}
	else //Otherwise, just go to your target player
	{
		MoveCloserToTargetPlayer(nullptr);
	}
}

void AEnemyBaseGridCharacter::ItemIsUnreachable(ATile* startingTile_)
{
	targetItem = nullptr;

	if (movementPath.Num() > 0)
		movementPath.Empty();

	if(btlManager->GetPhase()==BTL_ENM)
		MoveCloserToTargetPlayer(startingTile_);
}

void AEnemyBaseGridCharacter::DetectItem(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_,
	bool bFromSweep_,
	const FHitResult &sweepResult_)
{
	if (otherActor_ != nullptr && otherActor_ != this && otherComp_ != nullptr)
	{
		ACrowdItem* item_ = Cast<ACrowdItem>(otherActor_);
		if (item_)
		{
			crdItems.Push(item_);
		}

	}
}

void AEnemyBaseGridCharacter::TakeItem(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_,
	bool bFromSweep_,
	const FHitResult &sweepResult_)
{
	if (otherActor_ != nullptr && otherActor_ != this && otherComp_ != nullptr)
	{
		ACrowdItem* item_ = Cast<ACrowdItem>(otherActor_);

		if (item_) //If we've reached the item we planned to reach then obtain it and go to the closest player
		{
			//TODO
			//Get the item's value and update stats

			item_->Obtained(GetActorLocation());

			if (item_ != targetItem)//We've obtained an item that we did not mark, we need to let it tell the enemy that marked that it's gone
				item_->ItemWasObtainedByAnEnemyThatDidNotMarkIt();

			ItemIsUnreachable(item_->GetMyTile()); //You got the item, reset and go to the player starting from the item_ tile

			item_->Destroy();
		}
	}
}

AGridCharacter* AEnemyBaseGridCharacter::GetCurrentTarget()
{
	return targetCharacter;
}

void AEnemyBaseGridCharacter::GridCharTakeDamage(float damage_, AGridCharacter* attacker_, bool crit_)
{
	//Rotate to face attacker
	Super::GridCharTakeDamage(damage_, attacker_, crit_);

	damage_ = damage_ - ((static_cast<float>(statsComp->GetStatValue(STAT_DEF)) / (damage_ + static_cast<float>(statsComp->GetStatValue(STAT_DEF)))) * damage_);
	animInstance->SetDamage(static_cast<int> (damage_));
	overheadWidgetComp->SetVisibility(true);

	//update stats component
	statsComp->AddToStat(STAT_HP, static_cast<int>(-damage_));
	//Check if dead
	if (statsComp->GetStatValue(STAT_HP) <= 1)
	{
		GetMyTile()->SetOccupied(false);
		attacker_->YouHaveKilledYouTarget(false);
		if (statsComp->AddTempCRD(CRD_DED)) //You're dead so you also lose favor
		{
			crdManager->UpdateFavor(true);
		}

		if (aiManager)
			aiManager->HandleEnemyDeath(this);

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
	}
}

void AEnemyBaseGridCharacter::GridCharReactToSkill(float damage_, int statIndex_, int statuEffectIndex_, AGridCharacter* attacker_, bool crit_)
{
	Super::GridCharReactToSkill(damage_, statIndex_, statuEffectIndex_, attacker_, crit_);

	damage_ = damage_ - ((static_cast<float>(statsComp->GetStatValue(STAT_DEF)) / (damage_ + static_cast<float>(statsComp->GetStatValue(STAT_DEF)))) * damage_);
	animInstance->SetDamage(static_cast<int> (damage_));
	overheadWidgetComp->SetVisibility(true);
	//update stats component
	statsComp->AddToStat(STAT_HP, static_cast<int>(-damage_));
	//Check if dead
	if (statsComp->GetStatValue(STAT_HP) <= 1)
	{
		GetMyTile()->SetOccupied(false);
		attacker_->YouHaveKilledYouTarget(false);
		if (statsComp->AddTempCRD(CRD_DED)) //You're dead so you also lose favor
		{
			crdManager->UpdateFavor(true);
		}

		if (aiManager)
			aiManager->HandleEnemyDeath(this);

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
	}
}


void AEnemyBaseGridCharacter::CheckChangeStats()
{
	int changedStat = Intermediate::GetInstance()->GetStatsChange(CHG_STAT_ENM);
	if (changedStat != -1)
	{
		statsComp->AddToStat(changedStat, -static_cast<float>(statsComp->GetStatValue(changedStat))*PLY_IMP_STAT);
		animInstance->ChangeStats(false);
	}
}

///Summary of movement
/*
- The enemy's detectItems collider collides with items and those are pushed into the crdItems array.
- On the beginning of the enemy's turn, the enemy checks whether any of the items inside the array are reachable and can be marked (i.e. haven't been marked by a different enemy yet)
- If an enemy finds such an item, it'll go to it, once the collision occurs with the item, ItemIsUnreachable is called which calls MoveCloserToTargetPlayer and passes in the item's tile as the starting tile.
- The enemy starts moving from the item's tile to a tile close to the player and within the enemy's attack range.
- If the enemy does not find a reachable item, then they go straight to the player from their current tile.

*/
