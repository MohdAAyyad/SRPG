// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterTableStruct.h"

void FFighterTableStruct::CalculatePrice(int newLevel_)
{
	//TODO
	if (newLevel_ > level)
	{
		value += 100;
	}
	else if (newLevel_ < level)
	{
		value -= 100;
	}

}


void FFighterTableStruct::ScaleStatsByLevel(int newLevel_)
{
	//TODO
	//Placeholder
	if (newLevel_ > level)
	{
		hp += 20;
		spd += 2;
		atk += 3;
		def += 4;
		CalculatePrice(newLevel_);
		level = newLevel_;

	}
	else if (newLevel_ < level)
	{
		hp -= 20;
		spd -= 2;
		atk -= 3;
		def -= 4;
		CalculatePrice(newLevel_);
		level = newLevel_;

	}

}

void FFighterTableStruct::LevelUpUntilGoal(int goalLevel_)
{
	bool exit = false;

	if (goalLevel_ > level)
	{
		while (exit == false)
		{
			int increment = level + 1;
			ScaleStatsByLevel(increment);
			if (level == goalLevel_)
			{
				// get out, go home
				exit = true;
			}

		}
	}
	else if (goalLevel_ < level && level > 1)
	{
		while (exit == false)
		{
			int decrement = level - 1;
			ScaleStatsByLevel(decrement);
			if (level == goalLevel_)
			{
				// get out, go home
				exit = true;
			}

		}
	}

	level = goalLevel_;
}