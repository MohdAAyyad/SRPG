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
#include "Components/WidgetComponent.h"

void APlayerGridCharacter::Selected()
{
	if (widgetComp)
		widgetComp->GetUserWidgetObject()->AddToViewport();
}
void APlayerGridCharacter::NotSelected()
{
	if (widgetComp)
		widgetComp->GetUserWidgetObject()->RemoveFromViewport();

	if (originTile)
		originTile->GetGridManager()->ClearHighlighted();
}
void APlayerGridCharacter::HighlightMovementPath()
{
	if (originTile)
	{
		if (movementPath.Num() > 0)
			movementPath.Empty();
		if (pathComp)
			originTile->GetGridManager()->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth());
	}
}
void APlayerGridCharacter::HighlightRegularAttackPath()
{

}