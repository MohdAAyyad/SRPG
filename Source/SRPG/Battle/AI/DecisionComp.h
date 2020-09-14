// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DecisionComp.generated.h"

UENUM()
enum EPatterns
{
	DEFAULT = 0, //Gets close to a player and attacks
	ASSASSIN = 1, //Picks the player with the lowest health within its range
	FOLLOWER = 2, //Picks an enemy close by and follows them. Targets the same enemy they're targeting. Moves the same as DEFAULT if a target has not been selected yet.
	PEOPLEPERSON = 3, //Gets a buff when fellow enemies are close by and a nerf when not. Moves the same way as DEFAULT.
	HEALER = 4 //Uses buff and heal skills on fellow enemies
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SRPG_API UDecisionComp : public UActorComponent
{
	GENERATED_BODY()

protected:
	EPatterns currentPattern;
	bool bTargetIsHostile; //True for players, false for enemies. Changes based on what the pattern decides is the target
	class AAIManager* aiManager;
	class ABattleManager* btlManager;
	class AEnemyBaseGridCharacter* ownerEnemy;
	class AGridCharacter* currentTarget;
	float targetRadius; //Used by all the patterns besides Default. Targets need to be within this radius to be considered

public:	
	UDecisionComp();
	AGridCharacter* FindTheOptimalTarget();
	void UpdatePattrn(int level_, bool healer_); // Certain patterns are level gated. Hearlers are predetermined
	void SetRefs(AAIManager* ai_, ABattleManager* btl_, AEnemyBaseGridCharacter* enemy_);
	bool IsMyTargetHostile(); //Depends on the pattern. Used by enemies to know if they should use offensive or support skills
	
	AGridCharacter* FindAnotherTarget(AGridCharacter* target_);
protected:
	virtual void BeginPlay() override;

	AGridCharacter* FindDefaultTarget();
	AGridCharacter* FindDefaultTarget(TArray<APlayerGridCharacter*> deployedPlayers_,FVector myLoc_);
	AGridCharacter* FindAssassinTarget();
	AGridCharacter* FindFollowerTarget();
	AGridCharacter* PeoplePersonEffect();
	AGridCharacter* FindHealerTarget();


public:	
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};