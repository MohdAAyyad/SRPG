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

#ifndef STAT_WPI //Weapon index
#define STAT_WPI 12
#endif

#ifndef STAT_ARI //Armor index
#define STAT_ARI 13
#endif

#ifndef STAT_WPN //Actual equipped weapon
#define STAT_WPN 14
#endif

#ifndef STAT_ARM //Actual equipped armor
#define STAT_ARM 15
#endif

#ifndef STAT_ACC //Actual equipped accessory
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

#ifndef STAT_ASN //Armor skills num
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

#ifndef STAT_WPN_EFFECT //Weapon status effect
#define STAT_WPN_EFFECT 26
#endif

#ifndef STAT_ARM_EFFECT //Armor status effect resistance
#define STAT_ARM_EFFECT 27
#endif

#ifndef STAT_ACC_EFFECT //Acessory status effect resistance
#define STAT_ACC_EFFECT 28
#endif


#ifndef STAT_TEMP_ATK
#define STAT_TEMP_ATK 0
#endif

#ifndef STAT_TEMP_DEF
#define STAT_TEMP_DEF 1
#endif

#ifndef STAT_TEMP_INT
#define STAT_TEMP_INT 2
#endif

#ifndef STAT_TEMP_SPD
#define STAT_TEMP_SPD 3
#endif

#ifndef STAT_TEMP_CRT
#define STAT_TEMP_CRT 4
#endif

#ifndef STAT_TEMP_HIT
#define STAT_TEMP_HIT 5
#endif

#pragma endregion

#pragma region Champion
#ifndef CHAMP_ATK //Champion ATK
#define CHAMP_ATK 2
#endif

#ifndef CHAMP_DEF //Champion DEF
#define CHAMP_DEF -1
#endif

#ifndef CHAMP_INT //Champion INT
#define CHAMP_INT 2
#endif

#ifndef CHAMP_SDP //Champion SPD
#define CHAMP_SDP 1
#endif

#ifndef CHAMP_PIP //Champion PIP
#define CHAMP_PIP 1
#endif

#ifndef VILL_ATK //Villain ATK
#define VILL_ATK -2
#endif

#ifndef VILL_DEF //Villain DEF
#define VILL_DEF 3
#endif

#ifndef VILL_INT //Villain INT
#define VILL_INT -2
#endif

#ifndef VILL_SPD //Villain INT
#define VILL_SPD 2
#endif

#ifndef VILL_PIP //Villain INT
#define VILL_PIP -1
#endif

#pragma endregion

#pragma region Tiles

#ifndef TILE_SIZE
#define TILE_SIZE 100.0f
#endif

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

#ifndef TILE_ENMA //Enemy attack
#define TILE_ENMA 6
#endif

#ifndef TILE_SKL
#define TILE_SKL 7
#endif

#ifndef TILE_SKLT
#define TILE_SKLT 8
#endif

#ifndef TILE_CRD
#define TILE_CRD 9
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

#ifndef BTLPAWN_SPD
#define BTLPAWN_SPD 600.0f
#endif

#ifndef BTL_SHRD_RWRD_PER_UNIT //Shard reward per surviving unit
#define BTL_SHRD_RWRD_PER_UNIT 5
#endif

#pragma endregion

#pragma region Crowd

#ifndef CRD_THRESHOLD //If the temp CRD is higher than this threshold, then increase the CRD
#define CRD_THRESHOLD 100
#endif

#ifndef CRD_DEP
#define CRD_DEP 20
#endif

#ifndef CRD_ATK
#define CRD_ATK 30
#endif

#ifndef CRD_KIL
#define CRD_KIL 60
#endif

#ifndef CRD_DED
#define CRD_DED -100
#endif

#ifndef CRD_HYP
#define CRD_HYP 100
#endif

#ifndef CRD_SKL
#define CRD_SKL 5 //Taken per target hit
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

#ifndef CHG_STAT_ENM  //Change enemy stats
#define CHG_STAT_ENM 0
#endif

#ifndef CHG_STAT_PLY  //Change stat party index
#define CHG_STAT_PLY 1
#endif

#ifndef CHG_STAT_NON 
#define CHG_STAT_NON -1
#endif

#pragma endregion

#pragma region Dialogue

#ifndef THROWAWAYLINES_MIN
#define THROWAWAYLINES_MIN 0 
#endif

#ifndef THROWAWAYLINES_MAX
#define THROWAWAYLINES_MAX 8 
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


#pragma region Attacking

#ifndef ATKR_MISS
#define ATKR_MISS 0
#endif

#ifndef ATKR_HIT
#define ATKR_HIT 1
#endif

#ifndef ATKR_CRIT
#define ATKR_CRIT 3
#endif

#ifndef HIT_CRIT_BASE
#define HIT_CRIT_BASE 20 
#endif

#ifndef ATK_RWRD
#define ATK_RWRD 20 
#endif

#ifndef SKL_RWRD
#define SKL_RWRD 30 
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
#define RAD_ASSA 2000.0f
#endif

#ifndef RAD_FOL
#define RAD_FOL 200.0f
#endif

#ifndef RAD_PP //Radius for peopleperson
#define RAD_PP 150.0f
#endif

#ifndef RAD_SUPP
#define RAD_SUPP 2000.0f
#endif

#ifndef HP_ASSA //HP threshold for assassin
#define HP_ASSA 0.6f
#endif

#ifndef HP_SUPP //HP threshold for supports. If health is less than this, then heal.
#define HP_SUPP 0.5f
#endif

#pragma endregion

#pragma region Maps
#ifndef MAP_HUB 
#define MAP_HUB FName(TEXT("HubLevel_S11"))
#endif

#ifndef MAP_BATTLE 
#define MAP_BATTLE FName(TEXT("BTL_00_Test"))
#endif
#pragma endregion

#pragma region Status Effects


#ifndef EFFECT_RES_CHANCE_BASE
#define EFFECT_RES_CHANCE_BASE 15 //Character's resistance are added to this when checking whether the character will get affected or not
#endif

#ifndef EFFECT_RES_CHANCE_ARM_INC
#define EFFECT_RES_CHANCE_ARM_INC 25 //How much having an armor that increases resistnace affects the chance of resisting
#endif

#ifndef EFFECT_RES_CHANCE_ACC_INC
#define EFFECT_RES_CHANCE_ACC_INC 25 //How much having an accessory that increases resistnace affects the chance of resisting
#endif


#ifndef EFFECT_NONE
#define EFFECT_NONE 0
#endif

#ifndef EFFECT_BURN //Fire
#define EFFECT_BURN 1
#endif

#ifndef EFFECT_PARALYSIS //Electricity --> Cannot move
#define EFFECT_PARALYSIS 2
#endif

#ifndef EFFECT_POISON //Poison
#define EFFECT_POISON 3
#endif

#ifndef EFFECT_BLEEDING //Get damaged proportional to the number of tiles moved
#define EFFECT_BLEEDING 4
#endif

#ifndef EFFECT_FREEZE //Ice --> Cannot move
#define EFFECT_FREEZE 5
#endif

#ifndef EFFECT_SLOW //Slow --> Less speed
#define EFFECT_SLOW 6
#endif

#ifndef BURN_DAMAGE //Fire
#define BURN_DAMAGE 10.0f
#endif

#ifndef EXPLOSION_DAMAGE //Explosion
#define EXPLOSION_DAMAGE 20.0f
#endif

#ifndef POISON_DAMAGE //Poison
#define POISON_DAMAGE 10.0f
#endif

#ifndef BLEEDING_DAMAGE //Bleeding
#define BLEEDING_DAMAGE 2.0f
#endif

#ifndef SLOW_DAMAGE //Slow
#define SLOW_DAMAGE 3
#endif

#pragma endregion


#include "CoreMinimal.h"
