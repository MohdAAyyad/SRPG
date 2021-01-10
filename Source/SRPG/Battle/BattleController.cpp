// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleController.h"
#include "SRPGPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "GridCharacter.h"
#include "AI/EnemyBaseGridCharacter.h"
#include "Grid/Tile.h"
#include "BattlePawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "BattleManager.h"
#include "Grid/GridManager.h"
#include "Definitions.h"
#include "Kismet/GameplayStatics.h"
#include "Crowd/BattleCrowd.h"
#include "Battle/Grid/Obstacle.h"

ABattleController::ABattleController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	controlledCharacter = nullptr;
	bTargetingWithASkill = false;
	targetingRowSpeed = 0;
	targetingDepthSpeed = 0;
	targetingPure = 0;
	previosulyTargetedActor = nullptr;
	btlManager = nullptr;
	focusRate = 0.35f;

	bReDeployingUnit = false;
}

void ABattleController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (bTargetingWithASkill) //Player is targeting with a skill. We need to show the tiles that can be attacked by the skill.
	{
		TargetingWithASkill();
	}

	if (bReDeployingUnit)
	{
		if (controlledCharacter)
		{		
			FHitResult TraceHitResult;
			GetHitResultUnderCursor(ECC_Visibility, false, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			TraceHitResult.Location.Z += 300.0f;
			controlledCharacter->SetActorLocation(TraceHitResult.Location);
		}
	}
}

void ABattleController::SetupInputComponent()
{
	// set up gameplay key bindings
	APlayerController::SetupInputComponent();

	SetInputMode(FInputModeGameAndUI());
	InputComponent->BindAction("LeftMouse", IE_Pressed, this, &ABattleController::HandleMousePress);
	InputComponent->BindAction("Cancel", IE_Pressed, this, &ABattleController::CancelCommand);
}

void ABattleController::HandleMousePress()
{
	FHitResult hit;
	GetHitResultUnderCursor(ECC_Camera, false, hit);

	if (hit.bBlockingHit)
	{
		if (btlManager)
		{
			if (HasAuthority())
			{
				if (btlManager->GetPhase() == BTL_DEP) //Deployment phase
				{
					targetTile = Cast<ATile>(hit.Actor);
					if (targetTile) //Check if we press on a tile first
					{
						if (!bReDeployingUnit)
						{
							if (targetTile->GetHighlighted() == TILE_DEP && !targetTile->GetOccupied()) //Deployment highlight index
							{
								btlManager->DeplyUnitAtThisLocation(targetTile->GetActorLocation());
								targetTile->SetOccupied(true);
							}
						}
						else
						{
							if (controlledCharacter) //If I am redeploying, I should have a controlled character
							{
								if (targetTile->GetHighlighted() == TILE_DEP && !targetTile->GetOccupied()) //Deployment highlight index
								{
									//Move the character to the new tile
									bReDeployingUnit = false;
									FVector loc = targetTile->GetActorLocation();
									loc.Z += 50.0f;
									controlledCharacter->SetActorLocation(loc);
									targetTile->SetOccupied(true);
									controlledCharacter = nullptr;
								}
							}
						}
					}
					else
					{
						if (!bReDeployingUnit)
						{
							//Clicking on a character in dep phase means the player wishes to move the character
							controlledCharacter = Cast<AGridCharacter>(hit.Actor);
							if (controlledCharacter)
							{
								ATile* tile_ = controlledCharacter->GetMyTile();
								if (tile_)
								{
									if (tile_->GetHighlighted() == TILE_DEP) //Make sure the character's tile is a deployment tile which ensures this is a player character not an enemy character
									{
										bReDeployingUnit = true;
										tile_->SetOccupied(false);
									}
									else //Player clicked on an enemy. Reset.
									{
										controlledCharacter = nullptr;
									}
								}
							}
						}
					}
				}
				else
				{
					if (!controlledCharacter) //If we don't have a controlled character, control the character you just pressed on
					{
						controlledCharacter = Cast<AGridCharacter>(hit.Actor);
						if (controlledCharacter)
						{
							//Don't store a reference to a character who's finished their turn
						//	if (controlledCharacter->GetCurrentState() != AGridCharacter::EGridCharState::FINISHED)
								controlledCharacter->Selected();
							//else
								//controlledCharacter = nullptr;
						}//

					}
					else if (controlledCharacter->GetCurrentState() == AGridCharacter::EGridCharState::MOVING)
					{
						targetTile = Cast<ATile>(hit.Actor);
						if (targetTile)
						{
							if (targetTile->GetHighlighted() == TILE_MOV)
							{
								if (!targetTile->GetOccupied())//Move to this tile if there is nobody standing on it already
								{
									if (battlePawn)
										battlePawn->LockOnActor(controlledCharacter);

									controlledCharacter->MoveToThisTile(targetTile);
								}
							}
						}
						else
						{
							controlledCharacter->NotSelected();
							controlledCharacter = nullptr;
						}
					}
					else if (controlledCharacter->GetCurrentState() == AGridCharacter::EGridCharState::ATTACKING)
					{
						//Get the character
						AGridCharacter* targetChar = Cast<AGridCharacter>(hit.Actor);
						if (targetChar)
						{
							//Get the tile
							ATile* tile_ = targetChar->GetMyTile();
							if (tile_)
							{
								//See if the tile is within attack range
								if (tile_->GetHighlighted() == TILE_ATK)
								{
									FocusOnGridCharacter(controlledCharacter, focusRate);
									controlledCharacter->AttackUsingWeapon(targetChar, focusRate);
								}
							}
						}
						//Check for obstacles
						else
						{
							AObstacle* targetObstacle = Cast<AObstacle>(hit.Actor);

							if (targetObstacle)
							{
								if (targetObstacle->IsAnyOfMyTilesHighlighted(TILE_ATK)) //Make sure at least one the obstacle's tiles are within attack 
								{
									UE_LOG(LogTemp, Warning, TEXT("Attacking obstacle"));
									FocusOnGridCharacter(controlledCharacter, focusRate);
									controlledCharacter->AttackUsingWeapon(targetObstacle, focusRate);
								}
							}
						}
					}
					else if (controlledCharacter->GetCurrentState() == AGridCharacter::EGridCharState::SKILLING)
					{
						//Check if the player presses on a character
						//Get the character
						AGridCharacter* targetChar = Cast<AGridCharacter>(hit.Actor);
						if (targetChar)
						{
							//Get the tile
							ATile* tile_ = targetChar->GetMyTile();
							AObstacle* targetObstacle = nullptr;
							if (tile_)
							{
								//See if the tile is within attack range and has been targeted
								if (tile_->GetHighlighted() == TILE_SKLT)
								{
									TArray<AGridCharacter*> targetedCharacters;

									for (int i = 0; i < targetingTiles.Num(); i++)
									{
										//Get the character standing on the tile
										AGridCharacter* gchar = targetingTiles[i]->GetMyGridCharacter();
										if (gchar != nullptr && !targetedCharacters.Contains(gchar))
										{
											targetedCharacters.Push(gchar);
										}
										else if(!targetObstacle) //Otherwise check if there's an obstacle on top of the tile if we haven't already got one
										{
											targetObstacle = targetingTiles[i]->GetMyObstacle();
										}
									}
									UE_LOG(LogTemp, Warning, TEXT("Skilling obstacle as well"));
									FocusOnGridCharacter(controlledCharacter, focusRate);
									controlledCharacter->AttackUsingSkill(targetedCharacters, focusRate, targetObstacle);
									bTargetingWithASkill = false;
								}
							}
						}
						//Check for obstacles
						else
						{
							AObstacle* targetObstacle = Cast<AObstacle>(hit.Actor);

							if (targetObstacle)
							{
								if (targetObstacle->IsAnyOfMyTilesHighlighted(TILE_SKL)) //SKL not SKLT as we only need the obstacle to be targetable
								{
									UE_LOG(LogTemp, Warning, TEXT("Skilling obstacle"));
									FocusOnGridCharacter(controlledCharacter, focusRate);
									controlledCharacter->AttackUsingSkill(TArray<AGridCharacter*>(), focusRate, targetObstacle);
									bTargetingWithASkill = false;
								}
							}
						}
					}
					else if (controlledCharacter->GetCurrentState() == AGridCharacter::EGridCharState::HEALING)
					{
						AGridCharacter* targetChar = Cast<AGridCharacter>(hit.Actor);
						if (targetChar)
						{
							//Get the tile
							ATile* tile_ = targetChar->GetMyTile();
							if (tile_)
							{
								if (tile_->GetHighlighted() == TILE_ITM)
								{
									controlledCharacter->UseItemOnOtherChar(targetChar);
								}
							}
						}
					}
					else
					{
						//If the player presses on a tile, then check which characters are standing on the highlighted group of tiles and attack those
						ATile* tile_ = Cast<ATile>(hit.Actor);
						if (tile_)
						{
							if (tile_->GetHighlighted() == TILE_SKLT)
							{
								TArray<AGridCharacter*> targetedCharacters;

								for (int i = 0; i < targetingTiles.Num(); i++)
								{
									AGridCharacter* gchar = targetingTiles[i]->GetMyGridCharacter();
									if (gchar != nullptr && !targetedCharacters.Contains(gchar))
										targetedCharacters.Push(gchar);
								}
								if (targetedCharacters.Num() > 0)
								{
									FocusOnGridCharacter(controlledCharacter, focusRate);
									controlledCharacter->AttackUsingSkill(targetedCharacters, focusRate, nullptr);
									bTargetingWithASkill = false;
								}
							}
						}
						else
						{
							controlledCharacter->NotSelected();
							controlledCharacter = nullptr;
						}
					}

				}
			}
			else
			{
				targetTile = Cast<ATile>(hit.Actor);
				if (targetTile)
				{
					FVector loc = targetTile->GetActorLocation();
					loc.Z += 50.0f;
					if (battlePawn)
					{
						battlePawn->SpawnItems(btlManager, loc);
					}
				}
				//else
				//{
				//	AGridCharacter* gchar = Cast<AGridCharacter>(hit.Actor);
				//	if (gchar)
				//	{
				//		gchar->Selected();
				//	}
				//}
		    }
		}
	}
}

void ABattleController::SetBattlePawn(ABattlePawn* pawn_)
{
	battlePawn = pawn_;

	if (battlePawn)
	{
		if (InputComponent)
		{
			InputComponent->BindAxis("Up", battlePawn, &ABattlePawn::MoveUpDown);
			InputComponent->BindAxis("Right", battlePawn, &ABattlePawn::MoveRightLeft);
			InputComponent->BindAxis("Zoom", battlePawn, &ABattlePawn::Zoom);
		}
	}
}

void ABattleController::SetBattleManager(ABattleManager* btlManger_)
{
	btlManager = btlManger_;
}

void ABattleController::FocusOnGridCharacter(AGridCharacter* chr_, float rate_)
{
	if (chr_)
	{
		SetViewTargetWithBlend(chr_, rate_);
	}
}


void ABattleController::SetTargetingWithSkill(bool value_,int row_, int depth_, int pure_)
{
	bTargetingWithASkill = value_;
	targetingRowSpeed = row_;
	targetingDepthSpeed = depth_;
	targetingPure = pure_;
}

void ABattleController::TargetingWithASkill()
{
	FHitResult hit;
	GetHitResultUnderCursor(ECC_Camera, false, hit);
	if (hit.bBlockingHit)
	{
		if (hit.Actor != previosulyTargetedActor) //Only update when we actually move the mouse over new tiles/characters
		{
			previosulyTargetedActor = hit.Actor;

			int lastIndexOfCurrentlyHighlightedTiles;
			ATile* targetTile_ = nullptr;
			TArray<ATile*> allHighlightedTiles;
			//Check if we're hovering over a tile
			targetTile_ = Cast<ATile>(hit.Actor);
			if (targetTile_)
			{
				if (targetTile_->GetHighlighted() == TILE_SKL) //Make sure the tile is highlighted for skill usage
				{
					if (targetingTiles.Num() > 0)
					{
						for (int i = 0; i < targetingTiles.Num(); i++)
						{
							targetingTiles[i]->GoBackToPreviousHighlight();
						}
						targetingTiles.Empty();
					}
					//Get the first index of the tiles you're about to highlight
					lastIndexOfCurrentlyHighlightedTiles = targetTile_->GetGridManager()->GetHighlightedTiles().Num();
					targetTile_->GetGridManager()->UpdateCurrentTile(targetTile_, targetingRowSpeed, targetingDepthSpeed, TILE_SKLT, targetingPure);
					allHighlightedTiles = targetTile_->GetGridManager()->GetHighlightedTiles();
					//Push the tiles into the targetingTiles array
					for (int i = lastIndexOfCurrentlyHighlightedTiles; i < allHighlightedTiles.Num(); i++)
					{
						targetingTiles.Push(allHighlightedTiles[i]);

					}
				}
			}
			else //Next check if we're hovering over a character
			{
				AGridCharacter* targetChar = Cast<AGridCharacter>(hit.Actor);
				if (targetChar)
				{
					targetTile_ = targetChar->GetMyTile();
					if (targetTile_)
					{
						if (targetTile_->GetHighlighted() == TILE_SKL)
						{
							if (targetingTiles.Num() > 0)
							{
								//Reset the targeting tiles to highlight 6
								for (int i = 0; i < targetingTiles.Num(); i++)
								{
									targetingTiles[i]->GoBackToPreviousHighlight();
								}
								targetingTiles.Empty();
							}
							//Get the first index of the tiles you're about to highlight
							lastIndexOfCurrentlyHighlightedTiles = targetTile_->GetGridManager()->GetHighlightedTiles().Num();
							targetTile_->GetGridManager()->UpdateCurrentTile(targetTile_, targetingRowSpeed, targetingDepthSpeed, TILE_SKLT, targetingPure);
							allHighlightedTiles = targetTile_->GetGridManager()->GetHighlightedTiles();
							//Push the tiles into the targetingTiles array
							for (int i = lastIndexOfCurrentlyHighlightedTiles; i < allHighlightedTiles.Num(); i++)
							{
								targetingTiles.Push(allHighlightedTiles[i]);
							}
						}

					}
				}
				else
				{
					if (targetingTiles.Num() > 0)
					{
						//Reset the targeting tiles to highlight 6
						for (int i = 0; i < targetingTiles.Num(); i++)
						{
							targetingTiles[i]->GoBackToPreviousHighlight();
						}
						targetingTiles.Empty();
					}
				}
			}
		}
	}
}

void ABattleController::CancelCommand()
{
	//Return to the idle UI state
	if (controlledCharacter)
	{
		controlledCharacter->NotSelected();
		if (controlledCharacter->GetCurrentState() != AGridCharacter::EGridCharState::IDLE && controlledCharacter->GetCurrentState() != AGridCharacter::EGridCharState::FINISHED) //If the character is not idle, that means tiles are being highlighted so pressing cancel here means the user wants to go back to the UI
			controlledCharacter->Selected();
		else
			controlledCharacter = nullptr; //Reset
	}
}

void ABattleController::ResetViewLock()
{
	//Stop being locked on to a target
	if (battlePawn)
		battlePawn->ResetLock();
}

void ABattleController::ResetFocus()
{
	if (battlePawn)
		SetViewTargetWithBlend(battlePawn, focusRate);
}

void ABattleController::ResetControlledCharacter()
{
	if (controlledCharacter)
		controlledCharacter->NotSelected();
	controlledCharacter = nullptr;
}