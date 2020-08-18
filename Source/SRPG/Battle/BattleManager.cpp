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
	Intermediate::GetInstance()->ResetSelectedFighters();

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

	if (phase == BTL_ENM)	//Enemy phase
	{
		if (aiManager)
			aiManager->BeginEnemyTurn();
	}
	else if (phase == BTL_CRD)
	{
			if (crdManager)
				crdManager->StartCrowdPhase();
	}
	else if (phase > BTL_CRD) //When the crowd phase ends, go back to the player phase
	{
		phase = BTL_PLY;
		totalNumberOfPhasesElapsed++;
		for (int i = 0; i < deployedUnits.Num(); i++)
		{
			deployedUnits[i]->UpdateOriginTile();
		}
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
	numberOfUnitsDeployed++;
	tileLoc_.Z += 50.0f;
	if (bpidOfUnitToBeDeployedNext != -1)
	{
		APlayerGridCharacter* unit = GetWorld()->SpawnActor<APlayerGridCharacter>(fighters[bpidOfUnitToBeDeployedNext], tileLoc_, FRotator::ZeroRotator);
		if (unit)
		{
			TArray<int> stats_;
			stats_.Reserve(18);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].hp);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].pip);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].atk);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].def);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].intl);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].spd);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].crit);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].hit);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].crd);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].level);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].currentEXP);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].neededEXPToLevelUp);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].weaponIndex);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].armorIndex);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].equippedWeapon);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].equippedArmor);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].equippedAccessory);
			stats_.Push(selectedFighters[indexOfSelectedFighterInSelectedFighters].emitterIndex);


			unit->SetBtlAndCrdManagers(this,crdManager);
			unit->UpdateStats(stats_);
			deployedUnits.Push(unit);
		}

		//Remove the fighter from the ones that can be deployed
		if (indexOfSelectedFighterInSelectedFighters >= 0 && indexOfSelectedFighterInSelectedFighters < selectedFighters.Num())
		{
			selectedFighters.RemoveAt(indexOfSelectedFighterInSelectedFighters);
			if (selectedFighters.Num() < 1)
				EndDeployment();
		}

		if (widgetComp)
			widgetComp->GetUserWidgetObject()->AddToViewport();

		deployedFightersIndexes.Push(indexOfSelectedFighterInSelectedFighters);
		bpidOfUnitToBeDeployedNext = -1;
	}
}

void ABattleManager::EndDeployment()
{
	if (gridManager)
		gridManager->ClearHighlighted();

	if (crdManager)
	{
		crdManager->CalculateFavorForTheFirstTime();
		crdManager->AddCrowdWidgetToViewPort();
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