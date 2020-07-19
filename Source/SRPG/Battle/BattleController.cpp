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

ABattleController::ABattleController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	controlledCharacter = nullptr;
	bMovingCamera = false;
}

void ABattleController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void ABattleController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

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
		//UE_LOG(LogTemp, Warning, TEXT("Hit something"));
		if (!controlledCharacter) //If we don't have a controller character, see if we've 
		{
			controlledCharacter = Cast<AGridCharacter>(hit.Actor);
			if (controlledCharacter)
			{
				controlledCharacter->Selected();
				SetViewTargetWithBlend(controlledCharacter, 0.35f);
				if (battlePawn)
					battlePawn->SetUnderControl(false);
			}
			//UE_LOG(LogTemp, Warning, TEXT("Got player"));


		}
		else
		{
			targetTile = Cast<ATile>(hit.Actor);
			if (targetTile)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Got Tile"));
				// We hit a tile, move there
					// set flag to keep updating destination until released
				if (targetTile->GetHighlighted())
				{
					controlledCharacter->MoveToThisTile(targetTile);
				}
				else
				{
					controlledCharacter->NotSelected();
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