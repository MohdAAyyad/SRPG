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
	statsComp->AddToStat(STAT_WPN, equippedWeapon);
	statsComp->AddToStat(STAT_ARM, equippedArmor);
	statsComp->AddToStat(STAT_ACC, equippedAccessory);
	statsComp->ScaleLevelWithArchetype(Intermediate::GetInstance()->GetNextOpponent().level, Intermediate::GetInstance()->GetNextOpponent().archtype);
	AddEquipmentStats(2);
	decisionComp->SetRefs(aiManager, btlManager, this);
}

void ABlankEnemyBaseGridCharacter::AddEquipmentStats(int tableIndex_)
{
	//Get the best equipment you can equip from the table

	FEquipmentTableStruct weapon;
	FEquipmentTableStruct armor;
	FEquipmentTableStruct accessory;

	//Get the stats of the equipment and add them to the character's stats
	if (fileReader)
	{
		weapon = fileReader->GetEquipmentById(2, statsComp->GetStatValue(STAT_WPN), EQU_WPN, statsComp->GetStatValue(STAT_WPI));
		armor = fileReader->GetEquipmentById(3, statsComp->GetStatValue(STAT_ARM), EQU_ARM, statsComp->GetStatValue(STAT_ARI));
		accessory = fileReader->GetEquipmentById(4, statsComp->GetStatValue(STAT_ACC), EQU_ACC, -1);

		statsComp->UpdateMaxHpAndMaxPip(weapon.hp + armor.hp + accessory.hp, weapon.pip + armor.pip + accessory.pip);
		statsComp->AddToStat(STAT_HP, weapon.hp + armor.hp + accessory.hp);
		statsComp->AddToStat(STAT_PIP, weapon.pip + armor.pip + accessory.pip);
		statsComp->AddToStat(STAT_ATK, weapon.atk + armor.atk + accessory.atk);
		statsComp->AddToStat(STAT_DEF, weapon.def + armor.def + accessory.def);
		statsComp->AddToStat(STAT_INT, weapon.intl + armor.intl + accessory.intl);
		statsComp->AddToStat(STAT_SPD, weapon.spd + armor.spd + accessory.spd);
		statsComp->AddToStat(STAT_CRT, weapon.crit + armor.crit + accessory.crit);
		statsComp->AddToStat(STAT_HIT, armor.hit + accessory.hit); //Agility is not affected by the weapon
		statsComp->AddToStat(STAT_WHT, weapon.hit); //Weapon hit is affected by the weapon's hit
		statsComp->AddToStat(STAT_CRD, weapon.crd + armor.crd + accessory.crd);
		statsComp->AddToStat(STAT_WSI, weapon.skillsIndex);
		statsComp->AddToStat(STAT_WSN, weapon.skillsN);
		statsComp->AddToStat(STAT_ASI, armor.skillsIndex);
		statsComp->AddToStat(STAT_ASN, armor.skillsN);
		statsComp->AddToStat(STAT_WRS, weapon.range);
		statsComp->AddToStat(STAT_WDS, weapon.range + 1);
		statsComp->AddToStat(STAT_PURE, weapon.pure);
	}

	if (pathComp)
		pathComp->UpdateSpeed(statsComp->GetStatValue(STAT_SPD));
}
