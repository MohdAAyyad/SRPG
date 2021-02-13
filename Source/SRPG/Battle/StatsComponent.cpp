// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsComponent.h"
#include "Definitions.h"
#include "PathComponent.h"
#include "GridCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	currentStats.Reserve(29);
	tempCRD = 0;
	maxHP = 0;
	maxPip = 0;
	bLevelingUp = false;
	expOffset = 0.0f;
	addedEXP = 0;
	ownerChar = nullptr;
	newLevel = 0;

	turnsSinceLastStatChange.Reserve(6);
	tempStatChange.Reserve(6);
	activeStatusEffects.Reserve(6);
	turnsSinceStatusEffect.Reserve(2);

	for (int i = 0; i < 6; i++)
	{
		turnsSinceLastStatChange.Push(0);
		tempStatChange.Push(0);
	}

	for (int i = 0; i < 6; i++)
	{
		activeStatusEffects.Push(0);
		turnsSinceStatusEffect.Push(0);
	}


}

void UStatsComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bLevelingUp)
	{
		if (currentStats[STAT_EXP] > 0)
		{
			currentStats[STAT_EXP]-=10;
			addedEXP+=10;
			expPercentage += expOffset;
			if (addedEXP >= currentStats[STAT_NXP])
			{
				addedEXP = 0;
				expPercentage = 0.0f;
				bLevelingUp = false; //Assume we won't level up again
				CheckLevelUp(true); //Levelup

			}
		}
		else
		{
			bLevelingUp = false;
			CheckLevelUp(false); //Calls this again so that finish leveling up is called
		}
	}
}

void UStatsComponent::SetOwnerRef(AGridCharacter* ref_)
{
	newLevel = 1; //Starts at minimum level
	ownerChar = ref_;
}

void UStatsComponent::PushAStat(int statValue_)
{
	currentStats.Push(statValue_);
}

int UStatsComponent::CalculateHit(int otherAgi_)
{
	if (FMath::RandRange(0, 20) + currentStats[STAT_WHT] >= otherAgi_) //Hit
	{
		if (FMath::RandRange(0, 100) <= currentStats[STAT_CRT]) //Crit
		{
			return ATKR_CRIT;
		}

		return ATKR_HIT;
	}
	return ATKR_MISS;
}
void UStatsComponent::TakeDamageToStat(int stat_, int value_)
{
	if (stat_ >= 0 && stat_ < currentStats.Num())
	{
		switch (stat_)
		{
		case STAT_HP: //Lose health
			currentStats[STAT_HP] -= value_;
		default: //Handle debuffs
			currentStats[stat_] -= value_;
			if (currentStats[stat_] < 0)
				currentStats[stat_] = 0;
			break;
		}
	}
}
void UStatsComponent::AddToStat(int stat_, int value_)
{
	if (stat_ >= 0 && stat_ < currentStats.Num())
	{
		switch (stat_)
		{
		case STAT_HP: //Regain health
			currentStats[STAT_HP] += value_;
			if (currentStats[STAT_HP] > maxHP)
			{
				currentStats[STAT_HP] = maxHP;
			}	
			else if (currentStats[STAT_HP] < 0)
			{
				currentStats[STAT_HP] = 0;
			}
			break;
		case STAT_PIP: //Regain pips
			currentStats[STAT_PIP] += value_;
			if (currentStats[STAT_PIP] > maxPip)
			{
				currentStats[STAT_PIP] = maxPip;
			}
			else if (currentStats[STAT_PIP] < 0)
			{
				currentStats[STAT_PIP] = 0;
			}
			break;
		default: //Handle buffs and nerfs
			currentStats[stat_] += value_;
			break;
		}
	}
}
bool UStatsComponent::AddTempCRD(int value_) //True if the CRD stat is increased
{
	tempCRD += value_;
	if (tempCRD >= CRD_THRESHOLD)
	{
		tempCRD -= CRD_THRESHOLD;
		currentStats[STAT_CRD] += 1;

		return true;
	}
	return false;
}
void UStatsComponent::CheckLevelUp(bool hasLeveledUp_)
{
	if (currentStats[STAT_EXP] > 0 || hasLeveledUp_) //We might have leveled up but have no exp left, that's why the or is there
	{
		if (hasLeveledUp_)
		{
			//Levelup
			currentStats[STAT_NXP] *= 2;
			ScaleLevelWithArchetype(currentStats[STAT_LVL]+1,currentStats[STAT_ARCH]);
			if (currentStats[STAT_EXP] > 0) //We still have exp to earn
				CheckLevelUp(false); //Call the function again to calculate the expOffset again
			else
				FinishLevlingUp(); //No move EXP, we're done
		}
		else
		{
			expOffset = 10.0f / currentStats[STAT_NXP]; //The point here is to try to minimize the amount of divisions made.
			bLevelingUp = true;
		}
	}
	else
	{
		FinishLevlingUp(); //No move EXP, we're done
	}
}

void UStatsComponent::SkipLevelup()
{

	if (bLevelingUp)
	{
		bLevelingUp = false;
		FTimerHandle skipDelayHandle; //Delay a bit before skipping to avoid the race condition
		float skipDelay = 0.3f;
		GetWorld()->GetTimerManager().SetTimer(skipDelayHandle, this, &UStatsComponent::SkipLevelup, skipDelay, false);
	}
	else
	{
		if (currentStats[STAT_EXP] >= currentStats[STAT_NXP])
		{
			currentStats[STAT_EXP] -= currentStats[STAT_NXP];
			expPercentage = 1.0f;
			currentStats[STAT_NXP] *= 2;
			ScaleLevelWithArchetype(currentStats[STAT_LVL] + 1, currentStats[STAT_ARCH]);
			FTimerHandle timeToUpdateExpHandle;
			float timeToUpdateEXP = 0.4f;
			GetWorld()->GetTimerManager().SetTimer(timeToUpdateExpHandle, this, &UStatsComponent::SkipLevelup, timeToUpdateEXP, false);
		}
		else
		{
			expPercentage = static_cast<float>(currentStats[STAT_EXP]) / static_cast<float>(currentStats[STAT_NXP]);
			FinishLevlingUp();
		}
	}
}

void UStatsComponent::FinishLevlingUp()
{
	if (ownerChar)
	{
		ownerChar->PassInYourFinalStatsToTheIntermediate(currentStats);
	}
}
int UStatsComponent::GetStatValue(int stat_)
{
	if (stat_ >= 0 && stat_ < currentStats.Num())
	{
		return currentStats[stat_];
	}
	else if (stat_ == -1)
	{
		return maxHP;
	}
	else if (stat_ == -2)
	{
		return maxPip;
	}

	return 0;
}
void UStatsComponent::ScaleLevelWithArchetype(int targetLevel_, int archetype_)
{
	//Binary increase based on archetype
	// Non-archetype stats have 1 added every two levels while the archetype stat gets a 1 added for every level
	int currentLevel = currentStats[STAT_LVL];
	int increment = currentLevel % 2;
	while (currentLevel < targetLevel_)
	{
		increment = currentLevel % 2;
		maxHP += increment;
		maxPip += increment;
		currentStats[STAT_HP] = maxHP;
		currentStats[STAT_PIP] = maxPip;
		switch (archetype_)
		{
		case ARCH_ATK:
			currentStats[STAT_ATK]++;
			for (int i = STAT_ATK + 1; i < STAT_LVL; i++)
			{
				if(i != STAT_SPD) //Spd is not affected by leveling up
					currentStats[i] += increment;
			}
			break;
		case ARCH_DEF:
			currentStats[STAT_ATK]+=increment;
			currentStats[STAT_DEF]++;
			for (int i = STAT_DEF + 1; i < STAT_LVL; i++)
			{
				if (i != STAT_SPD) //Spd is not affected by leveling up
					currentStats[i] += increment;
			}
			break;
		case ARCH_INT:
			currentStats[STAT_ATK] += increment;
			currentStats[STAT_DEF] += increment;
			currentStats[STAT_INT]++;
			for (int i = STAT_INT + 1; i < STAT_LVL; i++)
			{
				if (i != STAT_SPD) //Spd is not affected by leveling up
					currentStats[i] += increment;
			}
			break;
		default:
			break;
		}

		currentLevel++;
	}
	currentStats[STAT_LVL] = targetLevel_;
	currentStats[STAT_ARCH] = archetype_;
}

void UStatsComponent::InitStatsAtZero()
{
	for (int i = 0; i < 29; i++)
		currentStats.Push(0);

	currentStats[STAT_LVL] = 1; //Min level
	currentStats[STAT_SPD] = 2; //Min speed
	maxHP = currentStats[STAT_HP] = 10; //Min health
	maxPip = currentStats[STAT_PIP] = 3; //Min pips
}


void UStatsComponent::UpdateMaxHpAndMaxPip(int hp_, int pip_)
{
	//Called when adding weapon stats which happens after updating the currentStats array
	maxHP = currentStats[STAT_HP] + hp_;
	maxPip = currentStats[STAT_PIP] + pip_;
}


int UStatsComponent::GetMaxHP()
{
	return maxHP;
}
int UStatsComponent::GetMaxPIP()
{
	return maxPip;
}

float UStatsComponent::GetHPPercentage()
{
	return (static_cast<float>(currentStats[STAT_HP]) / static_cast<float>(maxHP));
}

float UStatsComponent::GetPIPPercentage()
{
	return (static_cast<float>(currentStats[STAT_PIP]) / static_cast<float>(maxPip));
}

float UStatsComponent::GetEXPPercentage()
{
	return expPercentage;
}


void UStatsComponent::UpdateChampionVillainStats(bool champion_)
{
	if (champion_)
	{
		currentStats[STAT_HP] = maxHP;
		currentStats[STAT_ATK] += CHAMP_ATK;
		currentStats[STAT_DEF] += CHAMP_DEF;
		currentStats[STAT_INT] += CHAMP_INT;
		currentStats[STAT_SPD] += CHAMP_SDP;
		AddToStat(STAT_PIP,CHAMP_PIP);
	}
	else
	{
		currentStats[STAT_ATK] += VILL_ATK;
		currentStats[STAT_DEF] += VILL_DEF;
		currentStats[STAT_INT] += VILL_INT;
		currentStats[STAT_SPD] += VILL_SPD;
		AddToStat(STAT_PIP, VILL_PIP);
	}
}
void UStatsComponent::RevertChampionVillainStatsUpdate(bool champion_)
{
	if (champion_)
	{
		currentStats[STAT_ATK] -= CHAMP_ATK;
		currentStats[STAT_DEF] -= CHAMP_DEF;
		currentStats[STAT_INT] -= CHAMP_INT;
		currentStats[STAT_SPD] -= CHAMP_SDP;
	}
	else
	{
		currentStats[STAT_ATK] -= VILL_ATK;
		currentStats[STAT_DEF] -= VILL_DEF;
		currentStats[STAT_INT] -= VILL_INT;
		currentStats[STAT_SPD] -= VILL_SPD;
	}
}


void UStatsComponent::CheckStatBuffNerfStatus()//Checks whether buffs and nerfs should be negated
{
	for (int i = 0; i< tempStatChange.Num(); i++)
	{
		if (tempStatChange[i] != 0)
		{
			turnsSinceLastStatChange[i]--;
			if (turnsSinceLastStatChange[i] <= 0) //Buff/nerf has ended
			{
				//Reset the stat to its orignal value and reset the temp arrays
				int statIndex_ = ConvertTempStatIndexToStatIndex(i);
				AddToStat(statIndex_, -tempStatChange[i]);
				tempStatChange[i] = 0;
				turnsSinceLastStatChange[i] = 0;

				if(statIndex_ == STAT_SPD) //Make sure the path component is aware of the speed change
					if (ownerChar)
						ownerChar->GetPathComponent()->UpdateSpeed(currentStats[STAT_SPD]);
			}
		}
	}


	//Check for status effects
	for (int i = 0; i < activeStatusEffects.Num(); i++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("activeStatusEffects[i] %d"), activeStatusEffects[i]);
		if (activeStatusEffects[i] != 0)
		{
			turnsSinceStatusEffect[i]--;
			if (turnsSinceStatusEffect[i] <= 0)
			{
				//if(activeStatusEffects[i] == EFFECT_SLOW || activeStatusEffects[i] == EFFECT_FREEZE || activeStatusEffects[i] == EFFECT_PARALYSIS ) //If the effect was slow then make sure to update the speed. The speed will have been updated here because it's treated as a nerf
				//	if (ownerChar)
				//		ownerChar->GetPathComponent()->UpdateSpeed(currentStats[STAT_SPD]);
				activeStatusEffects[i] = EFFECT_NONE;
				turnsSinceStatusEffect[i] = 0;

			}
			else //Damage the character if the status effect causes damage
			{
				if (activeStatusEffects[i] == EFFECT_BURN)
				{
					if (ownerChar)
						ownerChar->GridCharReatToElemental(BURN_DAMAGE, EFFECT_NONE);
				}
				else if (activeStatusEffects[i] == EFFECT_POISON)
				{
					if (ownerChar)
						ownerChar->GridCharReatToElemental(POISON_DAMAGE, EFFECT_NONE);
				}
				else if (activeStatusEffects[i] == EFFECT_BLEEDING)
				{
					if (ownerChar)
						ownerChar->GridCharReatToElemental(BLEEDING_DAMAGE, EFFECT_NONE);
				}
			}
		}
	}
}
void UStatsComponent::AddTempToStat(int statIndex_, int value_)//Handle buffs nerfs
{
	int tempindex = ConvertStatIndexToTempStatIndex(statIndex_); //Get the temp index
	if (tempindex >= 0 && tempindex < tempStatChange.Num())
	{
		if (value_ * tempStatChange[tempindex] < 0) //If this stat has previously been nerfed/buffed and you want to negate it
		{
			//Negate the previous effect
			AddToStat(statIndex_, -tempStatChange[tempindex]);
			tempStatChange[tempindex] = 0;
			turnsSinceLastStatChange[tempindex] = 0;
		}
		else if (value_ * tempStatChange[tempindex] == 0) //No previous nerfs or buffs
		{
			//Store the value and turns to buff/nerf the stat
			tempStatChange[tempindex] = value_;
			turnsSinceLastStatChange[tempindex] = 3;
			if (currentStats[statIndex_] + value_ <= 0)
			{
				//A stat cannot go below 1
				int offsetToOne = 1 - (currentStats[statIndex_] + value_);
				value_ += offsetToOne;
				tempStatChange[tempindex] = value_;
			}
			AddToStat(statIndex_, value_); //Update the stat
		}
	}
	if(statIndex_ == STAT_SPD) //If the affected stat is speed, make sure to tell the path component
		if (ownerChar)
			ownerChar->GetPathComponent()->UpdateSpeed(currentStats[STAT_SPD]);
	//No double buffing or nerfing to the same stat

}

bool UStatsComponent::HasThisStatBeenBuffed(int statIndex_)
{
	//Called by support characters to see if stats have already been buffed
	int tempIndex = ConvertStatIndexToTempStatIndex(statIndex_);
	if (tempStatChange[tempIndex] > 0) //BUffed
		return true;
	tempStatChange[tempIndex] = 1; //This function is called only when a support enemy is trying to buff another enemy. Changing the value to 1 marks this enemy so this prevents two enemies from buffing the same enemy
	return false;
}
int UStatsComponent::ConvertStatIndexToTempStatIndex(int statIndex_)
{
	switch (statIndex_)
	{
	case STAT_ATK:
		return STAT_TEMP_ATK;
	case STAT_DEF:
		return STAT_TEMP_DEF;
	case STAT_INT:
		return STAT_TEMP_INT;
	case STAT_SPD:
		return STAT_TEMP_SPD;
	case STAT_CRT:
		return STAT_TEMP_CRT;
	case STAT_HIT:
		return STAT_TEMP_HIT;
	default:
		return STAT_TEMP_ATK;
	}
}

int UStatsComponent::ConvertTempStatIndexToStatIndex(int tempStatIndex_)
{
	switch (tempStatIndex_)
	{
	case STAT_TEMP_ATK :
		return STAT_ATK;
	case STAT_TEMP_DEF :
		return STAT_DEF;
	case STAT_TEMP_INT :
		return STAT_INT;
	case STAT_TEMP_SPD :
		return STAT_SPD;
	case STAT_TEMP_CRT :
		return STAT_CRT;
	case STAT_TEMP_HIT :
		return STAT_HIT;
	default:
		return STAT_ATK;
	}
}

void UStatsComponent::CheckIfAffectedByStatusEffect(int effect_)
{
	if (effect_ != EFFECT_NONE)
	{
		int resistanceChance = EFFECT_RES_CHANCE_BASE;
		int effectChance = FMath::RandRange(0, 101);

		//Check for the character's resistance to the effect
		if (currentStats[STAT_ARM_EFFECT] == effect_)
		{
			resistanceChance += EFFECT_RES_CHANCE_ARM_INC;
		}
		if (currentStats[STAT_ACC_EFFECT] == effect_)
		{
			resistanceChance += EFFECT_RES_CHANCE_ACC_INC;
		}

		if (effectChance > resistanceChance)
		{
			if (!activeStatusEffects.Contains(effect_))
			{
				activeStatusEffects.Push(effect_); //Need to push it here as this is read by the UI
				turnsSinceStatusEffect.Push(3);

				if (effect_ == EFFECT_FREEZE || effect_ == EFFECT_PARALYSIS)
				{
					AddTempToStat(STAT_SPD, -currentStats[STAT_SPD]); //Speed reduced to 1
				//	if (ownerChar)
					//	ownerChar->GetPathComponent()->UpdateSpeed(currentStats[STAT_SPD]);
				}
				else if (effect_ == EFFECT_SLOW)
				{
					AddTempToStat(STAT_SPD, -SLOW_DAMAGE); //Reduce the speed by the slow effect
					//if(ownerChar)
					//	ownerChar->GetPathComponent()->UpdateSpeed(currentStats[STAT_SPD]);
				}
			}
		}
	}
}