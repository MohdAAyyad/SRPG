// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterTableStruct.h"
#include "Definitions.h"


void FFighterTableStruct::ScaleStatsByLevelUp(int targetLevel_)
{

	//Binary increase based on archetype
	// Non-archetype stats have 1 added every two levels while the archetype stat gets a 1 added for every level
	int currentLevel = level;
	int increment = currentLevel % 2;
	while (currentLevel < targetLevel_)
	{
		increment = currentLevel % 2;
		hp += increment;
		pip += increment;
		neededEXPToLevelUp *= 2;
		value += 150;
		switch (archetype)
		{
		case ARCH_ATK:
			atk++;
			
			def += increment;
			intl += increment;
			crit += increment;
			agl += increment;
			crd += increment;

			break;
		case ARCH_DEF:
			def++;

			atk += increment;
			intl += increment;
			crit += increment;
			agl += increment;
			crd += increment;
			break;
		case ARCH_INT:
			intl++;

			atk += increment;
			def += increment;
			crit += increment;
			agl += increment;
			crd += increment;
			break;
		default:
			break;
		}

		currentLevel++;
	}
	level = targetLevel_;

}

void FFighterTableStruct::ScaleStatsByLevelDown(int targetLevel_)
{
	//Binary increase based on archetype
// Non-archetype stats have 1 added every two levels while the archetype stat gets a 1 added for every level
	int currentLevel = level;
	int increment = currentLevel % 2;
	while (currentLevel > targetLevel_)
	{
		increment = currentLevel % 2;
		hp -= increment;
		pip -= increment;
		neededEXPToLevelUp /= 2;
		value -= 150;
		switch (archetype)
		{
		case ARCH_ATK:
			atk--;

			def -= increment;
			intl -= increment;
			crit -= increment;
			agl -= increment;
			crd -= increment;

			break;
		case ARCH_DEF:
			def--;

			atk -= increment;
			intl -= increment;
			crit -= increment;
			agl -= increment;
			crd -= increment;
			break;
		case ARCH_INT:
			intl--;

			atk -= increment;
			def -= increment;
			crit -= increment;
			agl -= increment;
			crd -= increment;
			break;
		default:
			break;
		}

		currentLevel--;
	}
	level = targetLevel_;
}