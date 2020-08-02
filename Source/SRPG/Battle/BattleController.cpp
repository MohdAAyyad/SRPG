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

ABattleController::ABattleController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	controlledCharacter = nullptr;
	bMovingCamera = false;

	btlManager = nullptr;
}

void ABattleController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
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
			if (btlManager->GetPhase() == 0) //Deployment phase
			{
				targetTile = Cast<ATile>(hit.Actor);
				if (targetTile)
				{
					if (targetTile->GetHighlighted() == 3 && !targetTile->GetOccupied()) //Deployment highlight index
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
						if (targetTile->GetHighlighted() == 0)
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
							if (tile_->GetHighlighted() == 1)
							{
								controlledCharacter->AttackThisTarget(targetChar,false);
							}
							else if (tile_->GetHighlighted() == 6)
							{
								controlledCharacter->AttackThisTarget(targetChar, true);
							}
						}
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