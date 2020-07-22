// Fill out your copyright notice in the Description page of Project Settings.


#include "Fighter.h"

void FFighter::CalculatePrice(TArray<int> stats_)
{
	for (int i = 0; i <= 8; i++)
	{
		price += stats_[i] * 2;
	}
}

TArray<int> FFighter::ScaleStatsByLevel(int newLevel_, TArray<int> stats_)
{
	// hp = 0
	// pip = 1
	// atk = 2
	// def = 3
	// int = 4
	// spd = 5
	// crit = 6
	// hit = 7
	// crd = 8
	// level = 9

	// just augment by 1 for now
	for (int i = 0; i <= 9; i++)
	{
		stats_[i] += 1;
	}
	CalculatePrice(stats_);
	return stats_;
}

TArray<int> FFighter::LevelUpUntilGoal(int goalLevel_, TArray<int> stats_)
{
	TArray<int> stats = stats_;
	// run a loop and recursively call scale stats untill we reach the end
	// are we augmenting?
	bool exit = false;

	if (goalLevel_ > stats_[9])
	{
		while (exit == false)
		{
			int increment = stats[9] + 1;
			if (stats[9] == goalLevel_)
			{
				// get out, go home
				exit = true;
			}
			stats = ScaleStatsByLevel(increment, stats_);
		}
	}
	else if (goalLevel_ < stats_[9] && goalLevel_ > 0)
	{
		while (exit == false)
		{
			int decrement = stats[9] - 1;
			if (stats[9] == goalLevel_)
			{
				// get out, go home
				exit = true;
			}
			stats = ScaleStatsByLevel(decrement, stats_);
		}
	}

	return stats;

}
