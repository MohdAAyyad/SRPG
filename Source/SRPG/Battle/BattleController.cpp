// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleController.h"
#include "SRPGPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "GridCharacter.h"
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
					if (targetTile->GetHighlighted() == 3) //Deployment highlight index
					{
						btlManager->DeplyUnitAtThisLocation(targetTile->GetActorLocation());
					}
				}
			}
			if (btlManager->GetPhase() == 1) //Player phase
			{
				//UE_LOG(LogTemp, Warning, TEXT("Hit something"));
				if (!controlledCharacter) //If we don't have a controller character, see if we've 
				{
					controlledCharacter = Cast<AGridCharacter>(hit.Actor);
					if (controlledCharacter)
					{
						controlledCharacter->Selected();
						SetViewTargetWithBlend(controlledCharacter, 0.35f);
					}
					//UE_LOG(LogTemp, Warning, TEXT("Got player"));


				}
				else
				{
					targetTile = Cast<ATile>(hit.Actor);
					if (targetTile)
					{
						//UE_LOG(LogTemp, Warning, TEXT("Got Tile"));

						//0 Move 1 Attack 2 Heal
						if (targetTile->GetHighlighted() == 0)
						{
							controlledCharacter->MoveToThisTile(targetTile);
						}
						else if (targetTile->GetHighlighted() == 1)
						{
							controlledCharacter->AttackThisTile(targetTile);
						}
						else
						{
							controlledCharacter->NotSelected();
							//Move the battle pawn to where the character is
							if (battlePawn)
							{
								battlePawn->SetUnderControl(false);
								battlePawn->SetActorLocation(FVector(controlledCharacter->GetActorLocation().X,
									controlledCharacter->GetActorLocation().Y,
									battlePawn->GetActorLocation().Z));
							}

							controlledCharacter = nullptr;
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