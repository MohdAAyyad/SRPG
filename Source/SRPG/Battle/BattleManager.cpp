// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"
#include "BattleController.h"
#include "Engine/World.h"
#include "GridCharacter.h"
#include "Grid/GridManager.h"
#include "AI/AIManager.h"
#include "Components/WidgetComponent.h"

// Sets default values
ABattleManager::ABattleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	phase = 0;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;
	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	widgetComp->SetupAttachment(root);
	widgetComp->SetVisibility(false);
	totalNumberOfPhasesElapsed = 1;

}

// Called when the game starts or when spawned
void ABattleManager::BeginPlay()
{
	Super::BeginPlay();
	ABattleController* ctrl = Cast<ABattleController>(GetWorld()->GetFirstPlayerController());

	if (ctrl)
		ctrl->SetBattleManager(this);
	if (widgetComp)
		widgetComp->GetUserWidgetObject()->AddToViewport();
	if (aiManager)
		aiManager->SetBattleAndGridManager(this, gridManager);
	if (gridManager)
		gridManager->HighlightDeploymentTiles(rowIndexOfStartingTile, offsetOfStartingTile, deploymentRowSpeed, deploymentDepth);

}

// Called every frame
void ABattleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


int ABattleManager::GetPhase()
{
	return phase;
}

void ABattleManager::NextPhase()
{
	phase++;

	if (phase == 2)	//Enemy phase
	{
		if (aiManager)
			aiManager->BeginEnemyTurn();
	}
	else if (phase > 3) //When the crowd phase ends, go back to the player phase
	{
		phase = 1;
		totalNumberOfPhasesElapsed++;
		for (int i = 0; i < deployedUnits.Num(); i++)
		{
			deployedUnits[i]->UpdateOriginTile();
		}
	}
			
}
void ABattleManager::DeployThisUnitNext(int bpid_)
{
	if (numberOfUnitsDeployed < maxNumberOfUnitsToDeploy)
	{
		bpidOfUnitToBeDeployedNext = bpid_;
		if (bpidOfUnitToBeDeployedNext < fighters.Num() && bpidOfUnitToBeDeployedNext >= 0)
		{
			if (widgetComp)
				widgetComp->GetUserWidgetObject()->RemoveFromViewport();
		}
	}
}
void ABattleManager::DeplyUnitAtThisLocation(FVector tileLoc_) //Called from battle controller
{
	numberOfUnitsDeployed++;
	tileLoc_.Z += 50.0f;
	if (bpidOfUnitToBeDeployedNext != -1)
	{
		AGridCharacter* unit = GetWorld()->SpawnActor<AGridCharacter>(fighters[bpidOfUnitToBeDeployedNext], tileLoc_, FRotator::ZeroRotator);
		if (unit)
		{
			unit->SetBattleManager(this);
			deployedUnits.Push(unit);
		}

		//PLACEHOLDER
		if (selectedFighters.Contains(bpidOfUnitToBeDeployedNext))
		{
			selectedFighters.Remove(bpidOfUnitToBeDeployedNext);
			if (selectedFighters.Num() < 1)
				NextPhase();
		}

		if (widgetComp)
			widgetComp->GetUserWidgetObject()->AddToViewport();

		deployedFightersIndexes.Push(bpidOfUnitToBeDeployedNext);
		bpidOfUnitToBeDeployedNext = -1;
	}
}

void ABattleManager::EndDeployment()
{
	if (gridManager)
		gridManager->ClearHighlighted();
	NextPhase();
}

int ABattleManager::GetTotalNumberOfPhasesElapsed()
{
	return totalNumberOfPhasesElapsed;
}
