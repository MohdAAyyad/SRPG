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
#include "SimpleDecisionComp.h"
#include "AssasinDecisionComp.h"
#include "SupportDecisionComp.h"


AEnemyBaseGridCharacter::AEnemyBaseGridCharacter() :AGridCharacter()
{
	detectionRadius = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionRadius"));
	detectionRadius->SetupAttachment(RootComponent);
	detectionRadius->SetGenerateOverlapEvents(true);
	detectionRadius->SetCollisionProfileName("EnemyDetectionRadius");
	detectionRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	targetCharacter = nullptr;
	targetTile = nullptr;
	targetItem = nullptr;
	bWillMoveAgain = true;
	bCannotFindTile = false;
	weaponIndex = 0;
	armorIndex = 0;
	bHealer = false; //Changed to true if the decision tree used is that of a support character.
	bLookForANewTargetMidAttack = false;
	tree = EDecisionTrees::DISTANCEBASED;
	bPersistent = false;
}

void AEnemyBaseGridCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOriginTile();
	if (detectionRadius)
		detectionRadius->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBaseGridCharacter::DetectItem);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBaseGridCharacter::TakeItem);

	if (tree == EDecisionTrees::LEVELBASED)
		ChooseDecisionTreeBasedBasedOnLevel();


}

void AEnemyBaseGridCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCannotFindTile) //Necessary in case the enemy needed a new destiantion while transitioning between tiles
	{
		ATile* myTile_ = GetMyTile();

		if (myTile_ && bWillMoveAgain)
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

	//Create the decision component
	switch (tree)
	{
	case EDecisionTrees::DISTANCEBASED:
		decisionComp = NewObject<USimpleDecisionComp>(this, TEXT("Simple Decision Comp"));
		if (decisionComp)
		{
			decisionComp->RegisterComponent();
		}
		break;
	case EDecisionTrees::ASSASSIN:
		decisionComp = NewObject<UAssasinDecisionComp>(this, TEXT("Assassin Decision Comp"));
		if (decisionComp)
		{
			decisionComp->RegisterComponent();
		}
		break;
	case EDecisionTrees::FOLLOWER:
		break;
	case EDecisionTrees::PEOPLEPERSON:
		break;
	case EDecisionTrees::SUPPORT:
		decisionComp = NewObject<USupportDecisionComp>(this, TEXT("Support Decision Comp"));
		if (decisionComp)
		{
			decisionComp->RegisterComponent();
		}
		bHealer = true;
		break;
	}
	if (decisionComp)
	{
		decisionComp->SetRefs(aiManager, btlManager, this);
		decisionComp->SetPersistence(bPersistent);
	}
}

void AEnemyBaseGridCharacter::ChooseDecisionTreeBasedBasedOnLevel()
{
	int level_ = statsComp->GetStatValue(STAT_LVL);
	if (level_ >= 1 && level_ < PAT_ONE)
		tree = EDecisionTrees::DISTANCEBASED;
	else if (level_ >= PAT_ONE && level_ < PAT_TWO)
		tree = EDecisionTrees::ASSASSIN;
	else if (level_ >= PAT_TWO && level_ < PAT_THR)
		tree = EDecisionTrees::FOLLOWER;
	else if (level_ >= PAT_THR)
		tree = EDecisionTrees::PEOPLEPERSON;

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
		weapon = fileReader->GetEquipmentByLevel(2, statsComp->GetStatValue(STAT_LVL),EQU_WPN, statsComp->GetStatValue(STAT_WPI));
		armor = fileReader->GetEquipmentByLevel(3, statsComp->GetStatValue(STAT_LVL),EQU_ARM, statsComp->GetStatValue(STAT_ARI));
		accessory = fileReader->GetEquipmentByLevel(4, statsComp->GetStatValue(STAT_LVL),EQU_ACC,-1);
		
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
		statsComp->AddToStat(STAT_WPN_EFFECT, weapon.statusEffect);
		statsComp->AddToStat(STAT_ARM_EFFECT, armor.statusEffect);
		statsComp->AddToStat(STAT_ACC_EFFECT, accessory.statusEffect);
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
	bHasDoneAnAction = false;
	bLookForANewTargetMidAttack = false;
	statsComp->CheckStatBuffNerfStatus();
	UpdateOriginTile();
	targetItem = decisionComp->UpdateTargetItem(gridManager, originTile, pathComp->GetRowSpeed(), pathComp->GetDepth());

	if (targetItem) //If we have a target item, then go there
	{
		MoveToTheTileWithinRangeOfThisTile(nullptr, targetItem->GetMyTile());
	}
	else //Otherwise, just go to your target player
	{
		MoveCloserToTargetPlayer(nullptr);
	}
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
	int skillType = -1;
	if (gridManager)
	{
		if (targetCharacter)
		{
			ATile* myTile_ = GetMyTile();
			if (myTile_)
			{
				if (decisionComp)
				{
					//Check if we're using a skill
					if (decisionComp->GetWillUseSkill(skillType))
					{
						chosenSkill = decisionComp->GetChosenSkill();
						gridManager->ClearHighlighted();
						gridManager->UpdateCurrentTile(myTile_, chosenSkill.rge, chosenSkill.rge +1, TILE_ENMA, chosenSkill.pure);

						if (targetCharacter->GetMyTile()->GetHighlighted() == TILE_ENMA) //Is the target within the skill's range?
						{
							//Now clear the highlighted tiles and highlight new tiles based on the skill's own row and depth speeds starting from the target's tile
							gridManager->ClearHighlighted();
							gridManager->UpdateCurrentTile(targetCharacter->GetMyTile(), chosenSkill.rows, chosenSkill.depths, TILE_ENMA, chosenSkill.pure);
							//Get all the characters on top of the hilighted tiles
							TArray<ATile*> skillTiles = gridManager->GetHighlightedTiles();

							UE_LOG(LogTemp, Warning, TEXT("Skill tiles num: %d"), skillTiles.Num());
							UE_LOG(LogTemp, Warning, TEXT("Skill type is: %d"), skillType);
							for (int i = 0; i < skillTiles.Num(); i++)
							{
							
								if (skillType == 0)
								{
									//Offense skills only target player characters
									APlayerGridCharacter* tar = Cast<APlayerGridCharacter>(skillTiles[i]->GetMyGridCharacter());
									if (tar)
									{
										UE_LOG(LogTemp, Warning, TEXT("Found player tar"));
										if(!actionTargets.Contains(tar))
											actionTargets.Push(tar);
									}
								}
								else
								{
									//Defense skills only target enemy characters
									AEnemyBaseGridCharacter* tar = Cast<AEnemyBaseGridCharacter>(skillTiles[i]->GetMyGridCharacter());
									if (tar)
									{
										if (!actionTargets.Contains(tar))
											actionTargets.Push(tar);
									}
								}

							}
							UE_LOG(LogTemp, Warning, TEXT("Animation index: %d"), chosenSkill.animationIndex);
							btlCtrl->FocusOnGridCharacter(this, btlCtrl->focusRate);
							chosenSkillAnimIndex = chosenSkill.animationIndex;
							AttackUsingSkill(btlCtrl->focusRate);
						}
						else
						{
							bHasDoneAnAction = true;
							if (aiManager)
							{
								aiManager->FinishedAttacking();
							}

							if(decisionComp) //Look for new items on your next turn as someone may take your item on their turn
								decisionComp->ClearCrdItems();
						}
					}
					else
					{
						if (skillType != 1) //If the enemy intended to use a defensive skill, then that enemy was a support enemy and so was looking to support a fellow enemy and should not do regular attacks
						{
							//Regular attack
							gridManager->ClearHighlighted();
							gridManager->UpdateCurrentTile(myTile_, statsComp->GetStatValue(STAT_WRS), statsComp->GetStatValue(STAT_WDS), TILE_ENMA, statsComp->GetStatValue(STAT_PURE));
							if (targetCharacter)
							{
								if (targetCharacter->GetMyTile()->GetHighlighted() == TILE_ENMA) //Is the player within attack range?
								{
									btlCtrl->FocusOnGridCharacter(this, btlCtrl->focusRate);
									AttackUsingWeapon(targetCharacter, btlCtrl->focusRate);
								}
								else
								{
									bHasDoneAnAction = true;
									if (aiManager)
									{
										aiManager->FinishedAttacking();
									}

									if (decisionComp) //Look for new items on your next turn as someone may take your item on their turn
										decisionComp->ClearCrdItems();
								}
							}
						}
						else
						{
							bHasDoneAnAction = true;
							if (aiManager)
							{
								aiManager->FinishedAttacking();
							}

							if (decisionComp) //Look for new items on your next turn as someone may take your item on their turn
								decisionComp->ClearCrdItems();
						}
					}
				}
				
			}
			else //Could not find the tile, don't break, finish the turn instead
			{
				bHasDoneAnAction = true;
				if (aiManager)
				{
					aiManager->FinishedAttacking();
				}

				if (decisionComp)
					decisionComp->ClearCrdItems();
			}
		}
		else //If you don't have a target yet, finish the attacking phase
		{
			bHasDoneAnAction = true;
			if (aiManager)
			{
				aiManager->FinishedAttacking();
			}
			if (decisionComp)
				decisionComp->ClearCrdItems();
		}
	}
}

void AEnemyBaseGridCharacter::ActivateWeaponAttack()
{
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
			actionTargets[0]->GridCharTakeDamage(statsComp->GetStatValue(STAT_ATK) *critModifier, this, crit_, statsComp->GetStatValue(STAT_WPN_EFFECT));

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

	if (skillValue < 0) //If this is a buff the skill value will be less than zero and the sacling varaibles need to match the sign of the value
	{
		atkScaled *= -1.0f;
		intiScaled *= -1.0f;
	}
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
	GetWorld()->GetTimerManager().SetTimer(finishedAttackingHandle, aiManager, &AAIManager::FinishedAttacking, btlCtrl->focusRate * 3.5f, false);
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
						GetMyTile()->SetOccupied(true);
						aiManager->FinishedMoving();
					}
				}

				if (bLookForANewTargetMidAttack)
				{
					//We've moved to a new target in our attack phase. Now attempt the attack
					ExecuteChosenAction();
					bLookForANewTargetMidAttack = false;
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
		pathComp->GetPathToTargetTile(-1); //Get the whole path towards the target
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
			pathComp->AdjustPath(tPath[tileVecIndexes[0]], movementPath);
		}

		if (movementPath.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Movement path is larger than zero"));
			bMoving = true;
		}
		else
		{
			if (aiManager)
			{
				UE_LOG(LogTemp, Warning, TEXT("Called finished moving"));
				GetMyTile()->SetOccupied(true);
				aiManager->FinishedMoving();
			}
		}

	}
	else //No target tile. We're not moving
	{
		if (aiManager)
		{
			GetMyTile()->SetOccupied(true);
			aiManager->FinishedMoving();
		}
	}
	
}
void AEnemyBaseGridCharacter::ItemIsUnreachable(ATile* startingTile_)
{
	if (decisionComp)
		decisionComp->RemoveCrdItem(targetItem);
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
			if (decisionComp)
				decisionComp->AddCrdItem(item_);
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
			//Mark the item as obtained
			item_->Obtained(GetActorLocation());

			if (item_ != targetItem)//We've obtained an item that we did not mark, we need to let it tell the enemy that marked that it's gone
				item_->ItemWasObtainedByAnEnemyThatDidNotMarkIt();

			ItemIsUnreachable(item_->GetMyTile()); //You got the item, reset and go to the player starting from the item_ tile
			
						//Get the item's value and update stats
			if (statsComp)
			{
				int statIndex_ = item_->GetStat();
				if (statIndex_ == STAT_HP || statIndex_ == STAT_PIP)
				{
					statsComp->AddToStat(statIndex_, item_->GetValue());
				}
				else
				{
					statsComp->AddTempToStat(statIndex_, item_->GetValue());
				}
			}
			item_->Destroy();
		}
	}
}

AGridCharacter* AEnemyBaseGridCharacter::GetCurrentTarget()
{
	return targetCharacter;
}


void AEnemyBaseGridCharacter::GridCharReatToElemental(float damage_, int statusEffectIndex_)
{
	if (damage_ > 0)
	{
		damage_ = damage_ - ((static_cast<float>(statsComp->GetStatValue(STAT_DEF)) / (damage_ + static_cast<float>(statsComp->GetStatValue(STAT_DEF)))) * damage_);
		animInstance->SetDamage(static_cast<int> (damage_));
		overheadWidgetComp->SetVisibility(true);
		statsComp->AddToStat(STAT_HP, static_cast<int>(-damage_));


		if (statsComp->GetStatValue(STAT_HP) <= 1)
		{
			GetMyTile()->SetOccupied(false);
			for (int i = 0; i < TargetedByTheseCharacters.Num(); i++)
			{
				TargetedByTheseCharacters[i]->IamDeadStopTargetingMe();
			}
			if (statsComp->AddTempCRD(CRD_DED)) //You're dead so you also lose favor
			{
				crdManager->UpdateFavor(true);
			}

			if (aiManager)
				aiManager->HandleEnemyDeath(this, bHealer);

			if (animInstance)
				animInstance->DeathAnim();
		}

		//TODO
		//Handle champion and villain situation
	}
	statsComp->CheckIfAffectedByStatusEffect(statusEffectIndex_);
}
void AEnemyBaseGridCharacter::GridCharTakeDamage(float damage_, AGridCharacter* attacker_, bool crit_, int statusEffect)
{
	//Rotate to face attacker
	Super::GridCharTakeDamage(damage_, attacker_, crit_, statusEffect);

	if (attacker_ != this)
	{
		damage_ = damage_ - ((static_cast<float>(statsComp->GetStatValue(STAT_DEF)) / (damage_ + static_cast<float>(statsComp->GetStatValue(STAT_DEF)))) * damage_);
		animInstance->SetDamage(static_cast<int> (damage_));
		overheadWidgetComp->SetVisibility(true);

		statsComp->CheckIfAffectedByStatusEffect(statusEffect);

		//update stats component
		statsComp->AddToStat(STAT_HP, static_cast<int>(-damage_));
		//Check if dead
		if (statsComp->GetStatValue(STAT_HP) <= 1)
		{
			GetMyTile()->SetOccupied(false);

			for (int i = 0; i < TargetedByTheseCharacters.Num(); i++)
			{
				TargetedByTheseCharacters[i]->IamDeadStopTargetingMe();
			}
			attacker_->YouHaveKilledYouTarget(false);
			if (statsComp->AddTempCRD(CRD_DED)) //You're dead so you also lose favor
			{
				crdManager->UpdateFavor(true);
			}

			if (aiManager)
				aiManager->HandleEnemyDeath(this, bHealer);

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
	else
	{ //If the character killed itself, then just die and don't affect anytihng else
		if (aiManager)
			aiManager->HandleEnemyDeath(this, bHealer);

		if (animInstance)
			animInstance->DeathAnim();
	}
}

void AEnemyBaseGridCharacter::GridCharReactToSkill(float damage_, int statIndex_, int statuEffectIndex_, AGridCharacter* attacker_, bool crit_)
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
		overheadWidgetComp->SetVisibility(true);
		statsComp->AddToStat(STAT_HP, static_cast<int>(-damage_));
		//Check if dead
		if (statsComp->GetStatValue(STAT_HP) <= 1)
		{
			GetMyTile()->SetOccupied(false);
			for (int i = 0; i < TargetedByTheseCharacters.Num(); i++)
			{
				TargetedByTheseCharacters[i]->IamDeadStopTargetingMe();
			}
			Intermediate::GetInstance()->PushUnitToDead(fighterID);
			attacker_->YouHaveKilledYouTarget(false);
			if (statsComp->AddTempCRD(CRD_DED)) //You're dead so you also lose favor
			{
				crdManager->UpdateFavor(true);
			}
			if (aiManager)
				aiManager->HandleEnemyDeath(this, bHealer);
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
	else if ((statIndex_ == STAT_HP || statIndex_ == STAT_PIP) && damage_ < 0) //Healing skill
	{
		animInstance->ChangeStats(true);
		statsComp->AddToStat(statIndex_, static_cast<int>(-damage_));
	}
	else //Buff or nerf to a stat
	{
		if (damage_ < 0) //Buff (I know it's confusing)
			animInstance->ChangeStats(true);
		else
			animInstance->ChangeStats(false);

		statsComp->AddTempToStat(statIndex_, static_cast<int>(-damage_));
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

void AEnemyBaseGridCharacter::IamDeadStopTargetingMe()
{
	if (decisionComp)
		decisionComp->ResetCurrentTarget();

	targetCharacter = nullptr;

	if (!bHasDoneAnAction) //We have finished moving so we're in attack phase and we 1. have not attacked yet 2. have not told the ai manager that we're not gonna attack
	{
		bLookForANewTargetMidAttack = true;
		MoveCloserToTargetPlayer(nullptr);
	}
}

void AEnemyBaseGridCharacter::UpdateTargetCharacter(AGridCharacter* newTarget_)
{ //Called from the decision component when changing targets
	targetCharacter = newTarget_;
}

///Summary of movement
/*
- The enemy's detectItems collider collides with items and those are pushed into the crdItems array.
- On the beginning of the enemy's turn, the enemy checks whether any of the items inside the array are reachable and can be marked (i.e. haven't been marked by a different enemy yet)
- If an enemy finds such an item, it'll go to it, once the collision occurs with the item, ItemIsUnreachable is called which calls MoveCloserToTargetPlayer and passes in the item's tile as the starting tile.
- The enemy starts moving from the item's tile to a tile close to the player and within the enemy's attack range.
- If the enemy does not find a reachable item, then they go straight to the player from their current tile.

*/
