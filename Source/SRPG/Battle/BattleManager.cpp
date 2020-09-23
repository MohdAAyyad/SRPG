// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"
#include "BattleController.h"
#include "Engine/World.h"
#include "GridCharacter.h"
#include "Grid/GridManager.h"
#include "AI/AIManager.h"
#include "Components/WidgetComponent.h"
#include "Battle/Crowd/BattleCrowd.h"
#include "Definitions.h"
#include "Player/PlayerGridCharacter.h"
#include "Intermediary/Intermediate.h"
#include "BattleController.h"
#include "Grid/Tile.h"
#include "TimerManager.h"

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
	indexOfSelectedFighterInSelectedFighters = 0;

	bBattleHasEnded = false;
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


	selectedFighters = Intermediate::GetInstance()->GetSelectedFighters();
	numberOfUnitsDeployed = Intermediate::GetInstance()->GetCurrentDeploymentSize();
	maxNumberOfUnitsToDeploy = Intermediate::GetInstance()->GetMaxDeploymentSize();

	btlCtrl = Cast<ABattleController>(GetWorld()->GetFirstPlayerController());

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
	if (!bBattleHasEnded)
	{
		phase++;

		if (phase == BTL_ENM)	//Enemy phase
		{
			for (int i = 0; i < deployedUnits.Num(); i++)
			{
				deployedUnits[i]->EndPlayerTurn();
			}
			if (aiManager)
				aiManager->BeginEnemyTurn();
		}
		else if (phase == BTL_CRD)
		{
			if (crdManager)
			{
				if (totalNumberOfPhasesElapsed == 1)
				{
					crdManager->CalculateFavorForTheFirstTime();
					crdManager->AddCrowdWidgetToViewPort();
				}
				crdManager->StartCrowdPhase();
			}
		}
		else if (phase > BTL_CRD) //When the crowd phase ends, go back to the player phase
		{
			phase = BTL_PLY;
			totalNumberOfPhasesElapsed++;
			for (int i = 0; i < deployedUnits.Num(); i++)
			{
				deployedUnits[i]->StartPlayerTurn();
			}
		}

		if (btlCtrl)
			btlCtrl->ResetControlledCharacter();
	}
			
}
void ABattleManager::DeployThisUnitNext(int index_)
{
	//Index of the element inside selectedFighters
	if (index_ >= 0 && index_ < selectedFighters.Num())
	{
		//Used to keep track of which characters the player chooses to deploy
		if (numberOfUnitsDeployed < maxNumberOfUnitsToDeploy)
		{
			bpidOfUnitToBeDeployedNext = selectedFighters[index_].bpid;
			indexOfSelectedFighterInSelectedFighters = index_;
			if (bpidOfUnitToBeDeployedNext < fighters.Num() && bpidOfUnitToBeDeployedNext >= 0)
			{
				if (widgetComp)
					widgetComp->GetUserWidgetObject()->RemoveFromViewport();
			}
		}
	}
}
void ABattleManager::DeplyUnitAtThisLocation(FVector tileLoc_) //Called from battle controller
{
	//Actual deployment of characters. Called when the mouses clicks on a deployment tile
	if (bpidOfUnitToBeDeployedNext != -1)
	{
		numberOfUnitsDeployed++;
		tileLoc_.Z += 50.0f;
		APlayerGridCharacter* unit = GetWorld()->SpawnActor<APlayerGridCharacter>(fighters[bpidOfUnitToBeDeployedNext], tileLoc_, FRotator::ZeroRotator);
		if (unit)
		{
			
			unit->SetBtlAndCrdManagers(this,crdManager);
			unit->SetFighterIndex(indexOfSelectedFighterInSelectedFighters);
			unit->SetFighterName(selectedFighters[indexOfSelectedFighterInSelectedFighters].name);
			deployedUnits.Push(unit);
			unit->bpID = bpidOfUnitToBeDeployedNext;
		}
		if (widgetComp)
			widgetComp->GetUserWidgetObject()->AddToViewport();

		deployedFightersIndexes.Push(indexOfSelectedFighterInSelectedFighters);
		bpidOfUnitToBeDeployedNext = -1;
	}
}

int ABattleManager::GetBpidOfUnitToBeDeployedNext()
{
	return bpidOfUnitToBeDeployedNext;
}

void ABattleManager::EndDeployment()
{
	if (gridManager)
		gridManager->ClearHighlighted();

	Intermediate::GetInstance()->ResetSelectedFighters();

	for (int i = 0; i < deployedUnits.Num(); i++)
	{
		deployedUnits[i]->UpdateOriginTile();
	}

	NextPhase();
}

int ABattleManager::GetTotalNumberOfPhasesElapsed()
{
	return totalNumberOfPhasesElapsed;
}

float ABattleManager::GetTotalStatFromDeployedPlayers(int statIndex_)
{
	float total = 0.0f;
	for (int i = 0; i < deployedUnits.Num(); i++)
	{
		if(deployedUnits[i])
			total += deployedUnits[i]->GetStat(statIndex_);
	}

	return total;
}

AGridCharacter* ABattleManager::GetPlayerWithHighestStat(int statIndex_)
{
	int max = 0;
	AGridCharacter* result = nullptr;
	for (int i = 0; i < deployedUnits.Num(); i++)
	{
		if (deployedUnits[i])
		{
			if (deployedUnits[i]->GetStat(statIndex_) > max)
			{
				result = deployedUnits[i];
				max = deployedUnits[i]->GetStat(statIndex_);
			}
		}
	}

	return result;
}
AGridCharacter* ABattleManager::GetPlayerWithLowestStat(int statIndex_)
{
	int min = 10000;
	AGridCharacter* result = nullptr;
	for (int i = 0; i < deployedUnits.Num(); i++)
	{
		if (deployedUnits[i])
		{
			if (deployedUnits[i]->GetStat(statIndex_) < min)
			{
				result = deployedUnits[i];
				min = deployedUnits[i]->GetStat(statIndex_);
			}
		}
	}

	return result;
}

TArray<APlayerGridCharacter*> ABattleManager::GetDeployedPlayers()
{
	return deployedUnits;
}

void ABattleManager::HandlePlayerDeath(APlayerGridCharacter* player_)
{
	if (deployedUnits.Contains(player_))
	{
		deployedUnits.Remove(player_);
	}

	

	if (deployedUnits.Num() == 0)
	{
		EndBattle(false); //Defeat
	}
}

void ABattleManager::EndBattle(bool victory_)
{
	if (widgetComp->GetUserWidgetObject()->IsInViewport())
		widgetComp->GetUserWidgetObject()->RemoveFromViewport();

	phase = BTL_END;
	if (victory_)
	{
		//TODO
		//End on a victory

		if(EndWidgets[0])
			widgetComp->SetWidgetClass(EndWidgets[0]);
		widgetComp->GetUserWidgetObject()->AddToViewport();
		FTimerHandle timeToUpdateExpHandle;
		float timeToUpdateEXP = 0.7f;
		GetWorld()->GetTimerManager().SetTimer(timeToUpdateExpHandle, this, &ABattleManager::UpdatePlayerEXP, timeToUpdateEXP, false);
	}
	else
	{
		if (EndWidgets[1])
			widgetComp->SetWidgetClass(EndWidgets[1]);
		//TODO
		//End on a defeat
	}
}

void ABattleManager::UpdatePlayerEXP()
{
	for (int i = 0; i < deployedUnits.Num(); i++)
	{
		if (deployedUnits[i])
			deployedUnits[i]->UpdateCurrentEXP();
	}
}
