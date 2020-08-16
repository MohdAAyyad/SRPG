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

ABattleController::ABattleController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	controlledCharacter = nullptr;
	bTargetingWithASkill = false;
	targetingRowSpeed = 0;
	targetingDepthSpeed = 0;
	previosulyTargetedActor = nullptr;
	btlManager = nullptr;
}

void ABattleController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (bTargetingWithASkill) //Player is targeting with a skill. We need to show the tiles that can be attacked by the skill.
	{
		TargetingWithASkill();
	}
}

void ABattleController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	SetInputMode(FInputModeGameAndUI());

	InputComponent->BindAction("LeftMouse", IE_Pressed, this, &ABattleController::HandleMousePress);
	InputComponent->BindAction("ResetView", IE_Released, this, &ABattleController::ResetView);

}


float ABattleController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);

		return Distance;
	}
	return 0.0f;
}

void ABattleController::HandleMousePress()
{
	FHitResult hit;
	GetHitResultUnderCursor(ECC_Camera, false, hit);

	if (hit.bBlockingHit)
	{
		if (btlManager)
		{
			if (btlManager->GetPhase() == BTL_DEP) //Deployment phase
			{
				targetTile = Cast<ATile>(hit.Actor);
				if (targetTile)
				{
					if (targetTile->GetHighlighted() == TILE_DEP && !targetTile->GetOccupied()) //Deployment highlight index
					{
						btlManager->DeplyUnitAtThisLocation(targetTile->GetActorLocation());
						targetTile->SetOccupied(true);
					}
				}
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("Hit something"));
				if (!controlledCharacter) //If we don't have a controlled character, control the character you just pressed on
				{
					controlledCharacter = Cast<AGridCharacter>(hit.Actor);
					if (controlledCharacter)
					{
						controlledCharacter->Selected();
						SetViewTargetWithBlend(controlledCharacter, 0.35f);
					}
					//UE_LOG(LogTemp, Warning, TEXT("Got player"));


				}
				else if(controlledCharacter->GetCurrentState() == AGridCharacter::EGridCharState::IDLE)
				{
					targetTile = Cast<ATile>(hit.Actor);
					if (targetTile)
					{
						//UE_LOG(LogTemp, Warning, TEXT("Got Tile"));

						//0 Move 1 Attack 2 Heal
						if (targetTile->GetHighlighted() == TILE_MOV)
						{
							if(!targetTile->GetOccupied()) //Move to this tile if there is nobody standing on it already
								controlledCharacter->MoveToThisTile(targetTile);
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
								controlledCharacter->AttackUsingWeapon(targetChar);
							}
						}
					}
				}
				else if (controlledCharacter->GetCurrentState() == AGridCharacter::EGridCharState::SKILLING)
				{
					//Get the character
					AGridCharacter* targetChar = Cast<AGridCharacter>(hit.Actor);
					if (targetChar)
					{
						//Get the tile
						ATile* tile_ = targetChar->GetMyTile();
						if (tile_)
						{
							//See if the tile is within attack range and has been targeted
							if (tile_->GetHighlighted() == TILE_SKLT)
							{
								TArray<AGridCharacter*> targetedCharacters;

								for (int i = 0; i < targetingTiles.Num(); i++)
								{
									if (targetingTiles[i]->GetMyGridCharacter())
										targetedCharacters.Push(targetingTiles[i]->GetMyGridCharacter());
								}
								controlledCharacter->AttackUsingSkill(targetedCharacters);
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
								if (targetingTiles[i]->GetMyGridCharacter())
									targetedCharacters.Push(targetingTiles[i]->GetMyGridCharacter());
							}
							if (targetedCharacters.Num() > 0)
							{
								controlledCharacter->AttackUsingSkill(targetedCharacters);
								bTargetingWithASkill = false;
							}
						}
					}
					else
					{
							controlledCharacter->NotSelected();
					}
				}

			}
		}
	}
}
void ABattleController::ResetView()
{
	if (battlePawn)
	{
		SetViewTargetWithBlend(battlePawn, 0.3f);
		battlePawn->SetUnderControl(true);
	}
	if (controlledCharacter)
	{
		controlledCharacter->NotSelected();
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


void ABattleController::SetTargetingWithSkill(bool value_,int row_, int depth_)
{
	bTargetingWithASkill = value_;
	targetingRowSpeed = row_;
	targetingDepthSpeed = depth_;
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
					targetTile_->GetGridManager()->UpdateCurrentTile(targetTile_, targetingRowSpeed, targetingDepthSpeed, TILE_SKLT);
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
							targetTile_->GetGridManager()->UpdateCurrentTile(targetTile_, targetingRowSpeed, targetingDepthSpeed, TILE_SKLT);
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