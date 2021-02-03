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
#include "Kismet/GameplayStatics.h"
#include "UnrealNetwork.h"
#include "Grid/ObstaclesManager.h"
#include "../Audio/AudioMnager.h"
#include "ExternalFileReader/ExternalFileReader.h"

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
	
	fileReader = CreateDefaultSubobject<UExternalFileReader>(TEXT("File Reader"));
	
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
		aiManager->SetBattleGridCrdManagers(this, gridManager,crdManager);
	if (gridManager)
		gridManager->HighlightDeploymentTiles(rowIndexOfStartingTile, offsetOfStartingTile, deploymentRowSpeed, deploymentDepth);


	selectedFighters = Intermediate::GetInstance()->GetSelectedFighters();
	numberOfUnitsDeployed = Intermediate::GetInstance()->GetCurrentDeploymentSize();

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
		bHasUpdatedPhase = true; //The UI checks this to paly the phase text animation

		//Adding delay before activating the phase to allow for the UI to do its thing
		FTimerHandle nextPhaseHandle;
		GetWorld()->GetTimerManager().SetTimer(nextPhaseHandle, this, &ABattleManager::PhaseAction, 2.0f, false);
	}
			
}

void ABattleManager::PhaseAction()
{
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

	//We tell the obstacled maanger
	if (obstacleManager)
		obstacleManager->TellObstaclesAPhaseHasPassed(phase);
}
void ABattleManager::DeployThisUnitNext(int index_)
{
	//Index of the element inside selectedFighters
	if (index_ >= 0 && index_ < selectedFighters.Num())
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
			unit->SetFighterID(selectedFighters[indexOfSelectedFighterInSelectedFighters].id);
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
	if (audioMgr) //Start battle music
		audioMgr->SwitchMusic(1);
	if (gridManager)
		gridManager->ClearHighlighted();

	Intermediate::GetInstance()->ResetSelectedFighters();

	int changeStatsCheck = Intermediate::GetInstance()->GetAffecteParty();

	if (changeStatsCheck == CHG_STAT_PLY)
	{
		hubEvents.Push(CHG_STAT_PLY); //Used by the UI
		UE_LOG(LogTemp, Warning, TEXT("PLY"));
	}

	for (int i = 0; i < deployedUnits.Num(); i++)
	{
		deployedUnits[i]->UpdateOriginTile();
		if (changeStatsCheck == CHG_STAT_PLY) //Check if the players have a change of stats at the beginning of the battle
			deployedUnits[i]->CheckChangeStats();
	}

	if (changeStatsCheck == CHG_STAT_ENM) //Check if the enemies have a changeStatCheck
	{
		aiManager->TellEnemiesToCheckChangedStats();
		hubEvents.Push(CHG_STAT_ENM); //Needed for UI indicators
		UE_LOG(LogTemp, Warning, TEXT("ENM"));
	}

	if (changeStatsCheck == CHG_STAT_NON) //No change stats check, continue normally
	{
		//UE_LOG(LogTemp, Warning, TEXT("NON"));
		NextPhase();
	}
	else //Otherwise, delay the next phase a bit
	{
		FTimerHandle nextPhaseHandle;
		GetWorld()->GetTimerManager().SetTimer(nextPhaseHandle, this, &ABattleManager::NextPhase, 3.0f, false);

		Intermediate::GetInstance()->ResetChangeStats(); //Reset the values
	}
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

AGridCharacter* ABattleManager::GetPlayerWithHighestStat(int statIndex_, AGridCharacter* notThisCharacter_)
{
	int max = 0;
	AGridCharacter* result = notThisCharacter_; //In case we don't find another character, just return the original one
	for (int i = 0; i < deployedUnits.Num(); i++)
	{
		if (deployedUnits[i])
		{
			if (deployedUnits[i]->GetStat(statIndex_) > max && deployedUnits[i] != notThisCharacter_)
			{
				result = deployedUnits[i];
				max = deployedUnits[i]->GetStat(statIndex_);
			}
		}
	}

	return result;
}
AGridCharacter* ABattleManager::GetPlayerWithLowestStat(int statIndex_, AGridCharacter* notThisCharacter_)
{
	int min = 10000;
	AGridCharacter* result = notThisCharacter_; //In case we don't find another character, just return the original one
	for (int i = 0; i < deployedUnits.Num(); i++)
	{
		if (deployedUnits[i])
		{
			if (deployedUnits[i]->GetStat(statIndex_) < min && deployedUnits[i] != notThisCharacter_)
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
	if (widgetComp->GetUserWidgetObject())
		if (widgetComp->GetUserWidgetObject()->IsInViewport())
			widgetComp->GetUserWidgetObject()->RemoveFromViewport();

	phase = BTL_END;
	if (victory_)
	{
		//End on a victory

		if (audioMgr) //Start victory
			audioMgr->SwitchMusic(4);
		if(EndWidgets[0])
			widgetComp->SetWidgetClass(EndWidgets[0]);
		FTimerHandle timeToUpdateExpHandle;
		float timeToUpdateEXP = 0.7f;
		GetWorld()->GetTimerManager().SetTimer(timeToUpdateExpHandle, this, &ABattleManager::UpdatePlayerEXP, timeToUpdateEXP, false);

		if (fileReader)
		{
			FDayTableStruct dayInfo = fileReader->GetCurrentDayInfo(0, Intermediate::GetInstance()->GetCurrentDay() - 1);
			Intermediate::GetInstance()->Victory(dayInfo.moneyReward, dayInfo.shardsReward + deployedUnits.Num() * BTL_SHRD_RWRD_PER_UNIT);
		}
	}
	else
	{
		if (audioMgr) //Start defeat music
			audioMgr->SwitchMusic(5);

		//End on a defeat
		if (EndWidgets[1])
			widgetComp->SetWidgetClass(EndWidgets[1]);

		if (fileReader)
		{
			FDayTableStruct dayInfo = fileReader->GetCurrentDayInfo(0, Intermediate::GetInstance()->GetCurrentDay() - 1);
			Intermediate::GetInstance()->Defeat(dayInfo.retryMoneyCompensation, dayInfo.retryShardsCompensation);
		}
		
	}
	widgetComp->GetUserWidgetObject()->AddToViewport();
}

void ABattleManager::UpdatePlayerEXP()
{
	for (int i = 0; i < deployedUnits.Num(); i++)
	{
		if (deployedUnits[i])
			deployedUnits[i]->UpdateCurrentEXP();
	}
}

void ABattleManager::SpawnWeaponEmitter(FVector loc_, int emitterIndex_)
{
	if (emitterIndex_ >= 0 && emitterIndex_ < weaponEmitters.Num())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), weaponEmitters[emitterIndex_], loc_, FRotator::ZeroRotator);
	}
}

ABattleCrowd* ABattleManager::GetCrowdRef()
{
	return crdManager;
}

void ABattleManager::ActivateOutlines(bool value_)
{
	for (int i = 0; i < deployedUnits.Num(); i++)
	{
		if(deployedUnits[i])
			deployedUnits[i]->ActivateOutline(value_);
	}
	if (aiManager)
		aiManager->ActivateOutlines(value_);

	if(obstacleManager)
		obstacleManager->ActivateOutlines(value_);
}
