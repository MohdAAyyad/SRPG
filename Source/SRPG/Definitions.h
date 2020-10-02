// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#pragma region Stats

#ifndef STAT_HP
#define STAT_HP 0
#endif

#ifndef STAT_PIP
#define STAT_PIP 1
#endif

#ifndef STAT_ATK
#define STAT_ATK 2
#endif

#ifndef STAT_DEF
#define STAT_DEF 3
#endif

#ifndef STAT_INT
#define STAT_INT 4
#endif

#ifndef STAT_SPD
#define STAT_SPD 5
#endif

#ifndef STAT_CRT
#define STAT_CRT 6
#endif

#ifndef STAT_HIT
#define STAT_HIT 7
#endif

#ifndef STAT_CRD
#define STAT_CRD 8
#endif

#ifndef STAT_LVL
#define STAT_LVL 9
#endif

#ifndef STAT_EXP
#define STAT_EXP 10
#endif

#ifndef STAT_NXP
#define STAT_NXP 11
#endif

#ifndef STAT_WPI
#define STAT_WPI 12
#endif

#ifndef STAT_ARI
#define STAT_ARI 13
#endif

#ifndef STAT_WPN
#define STAT_WPN 14
#endif

#ifndef STAT_ARM
#define STAT_ARM 15
#endif

#ifndef STAT_ACC
#define STAT_ACC 16
#endif

#ifndef STAT_PURE //Weapon pure VH or not
#define STAT_PURE 17
#endif

#ifndef STAT_WHT //Weapon hit
#define STAT_WHT 18
#endif

#ifndef STAT_WSI //Weapon skills index
#define STAT_WSI 19
#endif

#ifndef STAT_WSN //Weapon skills num
#define STAT_WSN 20
#endif

#ifndef STAT_ASI//Armor skills index
#define STAT_ASI 21
#endif

#ifndef STAT_ASN
#define STAT_ASN 22
#endif

#ifndef STAT_WRS  //Weapon row speed
#define STAT_WRS 23
#endif

#ifndef STAT_WDS //Weapon row depth
#define STAT_WDS 24
#endif

#ifndef STAT_ARCH //Archetype
#define STAT_ARCH 25
#endif

#pragma endregion

#pragma region Tiles

#ifndef TILE_MOV
#define TILE_MOV 0
#endif

#ifndef TILE_ATK
#define TILE_ATK 1
#endif

#ifndef TILE_ITM
#define TILE_ITM 2
#endif

#ifndef TILE_DEP
#define TILE_DEP 3
#endif

#ifndef TILE_ENM
#define TILE_ENM 4
#endif

#ifndef TILE_ENMH
#define TILE_ENMH 5
#endif

#ifndef TILE_SKL
#define TILE_SKL 6
#endif

#ifndef TILE_SKLT
#define TILE_SKLT 7
#endif

#ifndef TILE_CRD
#define TILE_CRD 8
#endif

#pragma endregion

#pragma region BattlePhase

#ifndef BTL_DEP
#define BTL_DEP 0
#endif

#ifndef BTL_PLY
#define BTL_PLY 1
#endif

#ifndef BTL_ENM
#define BTL_ENM 2
#endif

#ifndef BTL_CRD
#define BTL_CRD 3
#endif

#ifndef BTL_END
#define BTL_END 4
#endif

#ifndef BTL_ACT_CHG_PLY // Change stats to improve the player activity
#define BTL_ACT_CHG_PLY 5
#endif

#ifndef BTL_ACT_CHG_ENM // Change stats affecting enemies
#define BTL_ACT_CHG_ENM 6
#endif

#pragma endregion

#pragma region Crowd

#ifndef CRD_DEP
#define CRD_DEP 10
#endif

#ifndef CRD_ATK
#define CRD_ATK 20
#endif

#ifndef CRD_KIL
#define CRD_KIL 40
#endif

#ifndef CRD_DED
#define CRD_DED -100
#endif

#ifndef CRD_HYP
#define CRD_HYP 100
#endif

#pragma endregion

#pragma region Archetypes

#ifndef ARCH_ATK
#define ARCH_ATK 0
#endif

#ifndef ARCH_DEF
#define ARCH_DEF 1
#endif

#ifndef ARCH_INT
#define ARCH_INT 2
#endif

#pragma endregion

#pragma region CentralNPCs

#ifndef IMP_CRD_LW //Improve CRD stats low value
#define IMP_CRD_LW 2
#endif

#ifndef IMP_CRD_HG //Improve CRD stats high value
#define IMP_CRD_HG 5
#endif

#ifndef PLY_IMP_STAT //Improve the player's stats due to changeStats activity
#define PLY_IMP_STAT 0.2f
#endif

#ifndef ENM_DEC_STAT //Decrease the enemy's stats due to changestats activity
#define ENM_DEC_STAT 0.2f
#endif

#ifndef CHG_STAT_PLY  //Change stat party index
#define CHG_STAT_PLY 1
#endif

#ifndef CHG_STAT_ENM 
#define CHG_STAT_ENM 0
#endif

#ifndef CHG_STAT_NON 
#define CHG_STAT_NON -1
#endif

#pragma endregion

#pragma region HubWorldManager

#ifndef CNTR_SPWN_LOW  // how many central NPCs to spawn, minimum
#define CNTR_SPWN_LOW 1
#endif

#ifndef CNTR_SPWN_HIGH
#define CNTR_SPWN_HIGH 3 // how many central NPCs to spawn, maximum
#endif


#ifndef BRNC_SPWN
#define BRNC_SPWN 20 // percent chance to spawn a branch NPC 
#endif

#ifndef ACT_INDX_LOW
#define ACT_INDX_LOW 1 // lowest activity index possible 
#endif

#ifndef ACT_INDX_HIGH
#define ACT_INDX_HIGH 3 // highest activity index possible
#endif


#pragma endregion


#pragma region AttackResults

#ifndef ATKR_MISS
#define ATKR_MISS 0
#endif

#ifndef ATKR_HIT
#define ATKR_HIT 1
#endif

#ifndef ATKR_CRIT
#define ATKR_CRIT 3
#endif

#pragma endregion

#pragma region EquipmentIndexes
#ifndef EQU_WPN
#define EQU_WPN 0
#endif

#ifndef EQU_ARM
#define EQU_ARM 1
#endif

#ifndef EQU_ACC
#define EQU_ACC 2
#endif

#pragma endregion

#pragma region EnemyPatterns

#ifndef PAT_ONE //The level to unlock the second (yes, second, we're counting from 0) enemy pattern. Not related the actual value in the enum
#define PAT_ONE 4
#endif

#ifndef PAT_TWO
#define PAT_TWO 8
#endif

#ifndef PAT_THR
#define PAT_THR 12
#endif

#ifndef RAD_ASSA //Radius for assassin
#define RAD_ASSA 200.0f;
#endif

#ifndef RAD_FOL
#define RAD_FOL 200.0f;
#endif

#ifndef RAD_PP //Radius for peopleperson
#define RAD_PP 150.0f;
#endif

#ifndef RAD_HEAL
#define RAD_HEAL 300.0f;
#endif

#ifndef HP_ASSA //HP threshold for assassin
#define HP_ASSA 0.6f;
#endif

#pragma endregion

#pragma region Maps
#ifndef MAP_HUB 
#define MAP_HUB FName(TEXT("HubLevel_S3"))
#endif

#ifndef MAP_BATTLE 
#define MAP_BATTLE FName(TEXT("BTL_00_Test"))
#endif
#pragma endregion

#include "CoreMinimal.h"
