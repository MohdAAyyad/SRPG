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


APlayerGridCharacter::APlayerGridCharacter() :AGridCharacter()
{
	attackRowSpeed = 2;
	attackDepth = 2;
}
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
		originTile->GetGridManager()->ClearHighlighted();

		if (movementPath.Num() > 0)
			movementPath.Empty();
		if (pathComp)
			originTile->GetGridManager()->UpdateCurrentTile(originTile, pathComp->GetRowSpeed(), pathComp->GetDepth(),0);
	}
}
void APlayerGridCharacter::HighlightRegularAttackPath()
{
		FHitResult hit;
		FVector end = GetActorLocation();
		ATile* tile;
		end.Z -= 400.0f;
		if (GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), end, ECollisionChannel::ECC_Visibility))
		{
			tile = Cast<ATile>(hit.Actor);
		}
		else
		{
			tile = originTile;
		}

		if (tile)
		{
			tile->GetGridManager()->ClearHighlighted();
			tile->GetGridManager()->UpdateCurrentTile(tile, attackRowSpeed, attackDepth, 1);
		}

}