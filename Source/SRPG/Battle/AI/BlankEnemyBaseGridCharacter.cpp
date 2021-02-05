// Fill out your copyright notice in the Description page of Project Settings.


#include "BlankEnemyBaseGridCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "BlankDecisionComp.h"
#include "../StatsComponent.h"
#include "Definitions.h"
#include "Intermediary/Intermediate.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Battle/PathComponent.h"

ABlankEnemyBaseGridCharacter::ABlankEnemyBaseGridCharacter() :AEnemyBaseGridCharacter()
{
	decisionComp = CreateDefaultSubobject<UBlankDecisionComp>(TEXT("Blank Decision Comp"));
}

void ABlankEnemyBaseGridCharacter::BeginPlay()
{
	AGridCharacter::BeginPlay();
	UpdateOriginTile();
	if (detectionRadius)
		detectionRadius->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBaseGridCharacter::DetectItem);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBaseGridCharacter::TakeItem);
}

void ABlankEnemyBaseGridCharacter::StartEnemyTurn()
{
	Super::StartEnemyTurn();
	if (decisionComp)
		decisionComp->ResetFunctionIndex();
}

void ABlankEnemyBaseGridCharacter::SetManagers(AAIManager* ref_, AGridManager* gref_, ABattleManager* bref_, ABattleCrowd* cref_)
{
	//Called by AI Manager on Beginplay
	aiManager = ref_;
	gridManager = gref_;
	btlManager = bref_;
	crdManager = cref_;

	statsComp->InitStatsAtZero();
	statsComp->AddToStat(STAT_WPI, weaponIndex);
	statsComp->AddToStat(STAT_ARI, armorIndex);
	statsComp->AddToStat(STAT_SPD, speed);
	statsComp->ScaleLevelWithArchetype(Intermediate::GetInstance()->GetNextOpponent().level, Intermediate::GetInstance()->GetNextOpponent().archtype);
	AddEquipmentStats(2);
	decisionComp->SetRefs(aiManager, btlManager, this);

	myHoverInfo.targetIndex = HOVER_ENM;
	myHoverInfo.hpPercentage = 1.0f;
	myHoverInfo.pipPercentage = 1.0f;
	myHoverInfo.textureID = bpID;
}
