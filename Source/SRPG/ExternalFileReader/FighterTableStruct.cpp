// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterTableStruct.h"

void FFighterTableStruct::CalculatePrice()
{
	//TODO
	value += 100;
}


void FFighterTableStruct::ScaleStatsByLevel(int newLevel_)
{
	//TODO
	//Placeholder
	hp += 20;
	spd += 2;
	atk += 3;
	def += 4;
	level = newLevel_;
	CalculatePrice();
}

void FFighterTableStruct::LevelUpUntilGoal(int goalLevel_)
{
	bool exit = false;

	if (goalLevel_ > level)
	{
		while (exit == false)
		{
			int increment = level + 1;
			if (level == goalLevel_)
			{
				// get out, go home
				exit = true;
			}
			ScaleStatsByLevel(increment);
		}
	}
	else if (goalLevel_ < level && level > 0)
	{
		while (exit == false)
		{
			int decrement = level - 1;
			if (level == goalLevel_)
			{
				// get out, go home
				exit = true;
			}
			ScaleStatsByLevel(decrement);
		}
	}

	level = goalLevel_;
}