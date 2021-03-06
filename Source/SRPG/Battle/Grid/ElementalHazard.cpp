// Fill out your copyright notice in the Description page of Project Settings.


#include "ElementalHazard.h"
#include "Components/BoxComponent.h"
#include "Definitions.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "ObstaclesManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Battle/GridCharacter.h"
#include "../StatsComponent.h"
#include "DestructibleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DecalActor.h"

AElementalHazard::AElementalHazard():AObstacle()
{
	currentStat = CurrentElemntalStat::NONE;
	turnsSinceStateChanged = 0;
	turnThatStatChangeOccuredIn = 0;
	particleLocation = FVector::ZeroVector;
	bDying = false;
	currentParticles = nullptr;
	PrimaryActorTick.bCanEverTick = true;
	explosionDamage = 15.0f;

	staticMesh->SetCollisionProfileName("OverlapAll");

	bShrinkingMesh = false;
	delayUntilDead = 2.0f;

	decalact = nullptr;
	
}

void AElementalHazard::BeginPlay()
{
	Super::BeginPlay();

	if (box)
		box->OnComponentBeginOverlap.AddDynamic(this, &AElementalHazard::OverlapWithGridCharacter);

	particleLocation = GetActorLocation();

	if (decal)
		decalact = GetWorld()->SpawnActor<ADecalActor>(GetActorLocation(), FRotator::ZeroRotator);

	if (decalact)
	{
		if (decalMaterial)
			decalact->SetDecalMaterial(decalMaterial);
	}
}

void AElementalHazard::Tick(float DeltaTime)
{
	if (bDying)
	{
		if (currentParticles)
		{
			UE_LOG(LogTemp, Warning, TEXT("Current particles"));
			//Shrink the size of the particles steadily then destroy them
			if (currentParticles->GetComponentScale().Size() > 0.3f)
			{
				UE_LOG(LogTemp, Warning, TEXT("Reducing size"));
				currentParticles->SetWorldScale3D(FMath::VInterpTo(currentParticles->GetComponentScale(), FVector::ZeroVector, DeltaTime, 1.0f));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Ded"));
				bDying = false;
				currentParticles->SetActive(false);
				currentParticles->DestroyComponent();
				Destroy(this);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Destroy this"));
			Destroy(this);
		}
	}
	else if (bShrinkingMesh)
	{
		/*if (staticMesh)
		{
			if (staticMesh->GetComponentScale().Size() > 0.1f)
			{
				UE_LOG(LogTemp, Warning, TEXT("Shrinking"));
				staticMesh->SetWorldScale3D(FMath::VInterpTo(currentParticles->GetComponentScale(), FVector::ZeroVector, DeltaTime, 1.0f));
			}
			else
			{
				bShrinkingMesh = false;
				staticMesh->SetActive(false);
				staticMesh->DestroyComponent();
			}
		}
		*/
	}

}

void AElementalHazard::ObstacleTakeDamage(float damage_, int statusEffect_)
{
	//Switch state according to the current and incoming status effects
	//Turn off collider and turn it back on to detect any characters on top of it

	//If we have particles already, that means we're changing the state, so we should destroy the original one first
	if (currentParticles)
		currentParticles->DestroyComponent();
	switch (currentStat)
	{
		case CurrentElemntalStat::OIL:
			if (statusEffect_ == EFFECT_BURN)
			{
				currentStat = CurrentElemntalStat::FIRE;
				/*if (box)
				{
					box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

					//Spawn fire -- Saved
					//Ask the obstacle manager for the current phase
					if (obstacleManager)
					{
						bShrinkingMesh = true;
						currentParticles = obstacleManager->SpawnElemntalEmitterAtLocation(particleLocation, currentStat);
						turnThatStatChangeOccuredIn = obstacleManager->GetBattlePhase();
						turnsSinceStateChanged = 3;
					}
					
				}*/
				
				//Changing the elementals to work like the explosion
				if (box)
				{
					box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					FVector scale_ = box->GetComponentScale();
					box->SetWorldScale3D(scale_*3.0f);
					box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

					if (obstacleManager)
					{
						bShrinkingMesh = true;
						currentParticles = obstacleManager->SpawnElemntalEmitterAtLocation(particleLocation, currentStat);


						if (decalact)
							decalact->Destroy();
					}
					//Give the explosion some time to play out then destroy the object
					FTimerHandle timeToDestroyHandle;
					GetWorld()->GetTimerManager().SetTimer(timeToDestroyHandle, this, &AElementalHazard::DelayedDestroy, delayUntilDead, false);
				}
			}
			break;
		case CurrentElemntalStat::WATER:

			if (statusEffect_ == EFFECT_BURN)
			{
				currentStat = CurrentElemntalStat::NONE;
				if(box)
					box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				
		        //Spawn steam
				//Destroy elemental object
				if (obstacleManager)
				{
					bShrinkingMesh = true;
					currentParticles = obstacleManager->SpawnElemntalEmitterAtLocation(particleLocation, CurrentElemntalStat::STEAM);
					if (decalact)
						decalact->Destroy();
				}
				FTimerHandle timeToDestroyHandle;
				GetWorld()->GetTimerManager().SetTimer(timeToDestroyHandle, this, &AElementalHazard::DelayedDestroy, delayUntilDead, false);


			}
			else if (statusEffect_ == EFFECT_FREEZE)
			{
				currentStat = CurrentElemntalStat::FROZEN;
				/*(if (box)
				{
					box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

					//Spawn Ice -- Saved
					//Ask the obstacle manager for the current phase
					if (obstacleManager)
					{
						bShrinkingMesh = true;
						currentParticles = obstacleManager->SpawnElemntalEmitterAtLocation(particleLocation, currentStat);
						turnThatStatChangeOccuredIn = obstacleManager->GetBattlePhase();
						turnsSinceStateChanged = 3;

					}
				}
				*/

				if (box)
				{
					box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					FVector scale_ = box->GetComponentScale();
					box->SetWorldScale3D(scale_*3.0f);
					box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

					if (obstacleManager)
					{
						bShrinkingMesh = true;
						currentParticles = obstacleManager->SpawnElemntalEmitterAtLocation(particleLocation, currentStat);

						if (decalact)
							decalact->Destroy();
					}
					//Give the explosion some time to play out then destroy the object
					FTimerHandle timeToDestroyHandle;
					GetWorld()->GetTimerManager().SetTimer(timeToDestroyHandle, this, &AElementalHazard::DelayedDestroy, delayUntilDead, false);
				}

			}
			else if (statusEffect_ == EFFECT_PARALYSIS)
			{
				currentStat = CurrentElemntalStat::PARALYSIS;
				/*if (box)
				{
					box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

					//Spawn Thunder -- Saved
					//Ask the obstacle manager for the current phase

					if (obstacleManager)
					{
						bShrinkingMesh = true;
						currentParticles = obstacleManager->SpawnElemntalEmitterAtLocation(particleLocation, currentStat);
						turnThatStatChangeOccuredIn = obstacleManager->GetBattlePhase();
						turnsSinceStateChanged = 3;

					}
				}
				*/

				if (box)
				{
					box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					FVector scale_ = box->GetComponentScale();
					box->SetWorldScale3D(scale_*3.0f);
					box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

					if (obstacleManager)
					{
						bShrinkingMesh = true;
						currentParticles = obstacleManager->SpawnElemntalEmitterAtLocation(particleLocation, currentStat);

						if (decalact)
							decalact->Destroy();
					}
					//Give the explosion some time to play out then destroy the object
					FTimerHandle timeToDestroyHandle;
					GetWorld()->GetTimerManager().SetTimer(timeToDestroyHandle, this, &AElementalHazard::DelayedDestroy, delayUntilDead, false);
				}
			}
			break;
		case CurrentElemntalStat::POISON:
			if (statusEffect_ == EFFECT_BURN)
			{
				//Play explosion particles

				currentStat = CurrentElemntalStat::EXPLOSION;
				if (box)
				{
					box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					FVector scale_ = box->GetComponentScale();
					box->SetWorldScale3D(scale_*3.0f);
					box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

					if (obstacleManager)
					{
						bShrinkingMesh = true;
						obstacleManager->SpawnElemntalEmitterAtLocation(particleLocation, currentStat);

						if (decalact)
							decalact->Destroy();
					}
					//Give the explosion some time to play out then destroy the object
					FTimerHandle timeToDestroyHandle;
					GetWorld()->GetTimerManager().SetTimer(timeToDestroyHandle, this, &AElementalHazard::DelayedDestroy, delayUntilDead, false);
				}

			}
			break;
		case CurrentElemntalStat::FIRE:

			if (statusEffect_ == EFFECT_FREEZE)
			{
				currentStat = CurrentElemntalStat::NONE;
				if (box)
					box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				//Spawn steam

				if (obstacleManager)
				{
					bShrinkingMesh = true;
					obstacleManager->SpawnElemntalEmitterAtLocation(particleLocation, CurrentElemntalStat::STEAM);

					if (decalact)
						decalact->Destroy();
				}
				FTimerHandle timeToDestroyHandle;
				GetWorld()->GetTimerManager().SetTimer(timeToDestroyHandle, this, &AElementalHazard::DelayedDestroy, delayUntilDead, false);

			}
			else if (statusEffect_ == EFFECT_POISON)
			{
				//Play explosion particles

				currentStat = CurrentElemntalStat::EXPLOSION;
				if (box)
				{
					box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					FVector scale_ = box->GetComponentScale();
					box->SetWorldScale3D(scale_*3.0f);
					box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					

					if (obstacleManager)
					{
						bShrinkingMesh = true;
						obstacleManager->SpawnElemntalEmitterAtLocation(particleLocation, currentStat);

						if (decalact)
							decalact->Destroy();
					}

					//Give the explosion some time to play out then destroy the object
					FTimerHandle timeToDestroyHandle;
					GetWorld()->GetTimerManager().SetTimer(timeToDestroyHandle, this, &AElementalHazard::DelayedDestroy, delayUntilDead, false);
				}
			}
			break;
		case CurrentElemntalStat::FROZEN:
			if (statusEffect_ == EFFECT_BURN)
			{
				currentStat = CurrentElemntalStat::NONE;
				if (box)
					box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				//Spawn steam
				if (obstacleManager)
				{
					bShrinkingMesh = true;
					obstacleManager->SpawnElemntalEmitterAtLocation(particleLocation, CurrentElemntalStat::STEAM);

					if (decalact)
						decalact->Destroy();
				}
				FTimerHandle timeToDestroyHandle;
				GetWorld()->GetTimerManager().SetTimer(timeToDestroyHandle, this, &AElementalHazard::DelayedDestroy, delayUntilDead, false);

			}
			break;
		case CurrentElemntalStat::PARALYSIS:
			break;
		default:
			break;
	}
}

bool AElementalHazard::AddObstructedTile(ATile* tile_)
{
	if (!obstructedTiles.Contains(tile_))
	{
		obstructedTiles.Push(tile_);
	}

	return false; //Tiles are still traversable
}

void AElementalHazard::DelayedDestroy()
{
	box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Decrease the size of the particle effect until it is very small then destroy it if the current state is not none or explosion. Otherwise, just get destroyed
	/*if (currentStat == CurrentElemntalStat::EXPLOSION || currentStat == CurrentElemntalStat::STEAM || currentStat == CurrentElemntalStat::NONE)
	{
		if (obstacleManager)
			obstacleManager->RemoveObstacle(this);
	}
	else
	{
		bDying = true;
	}
	*/

	if (obstacleManager)
		obstacleManager->RemoveObstacle(this);

	if (currentParticles)
	{
		currentParticles->SetActive(false);
		currentParticles->DestroyComponent();
	}
	Destroy(this);
}

void AElementalHazard::OverlapWithGridCharacter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{

	//Check for grid characters then apply status effect. No damage except for explosion

	if (currentStat != CurrentElemntalStat::NONE && currentStat != CurrentElemntalStat::WATER && currentStat != CurrentElemntalStat::STEAM && currentStat != CurrentElemntalStat::OIL)
	{
		if (OtherActor != nullptr && OverlappedComp != nullptr && OtherActor != this)
		{
			AGridCharacter* gChar = Cast<AGridCharacter>(OtherActor);
			if (gChar)
			{
				
				UStatsComponent* statsComp = gChar->GetStatsComp();

				if (currentStat == CurrentElemntalStat::EXPLOSION) //Explosion does damage and burn effect
				{
					gChar->GridCharReatToElemental(EXPLOSION_DAMAGE, EFFECT_BURN);
				}
				else //Apply the stat after checking the character's resistances
				{
					switch (currentStat)
					{
					case CurrentElemntalStat::OIL:
						gChar->GridCharReatToElemental(0.0f, EFFECT_SLOW);
						break;
					case CurrentElemntalStat::POISON:
						gChar->GridCharReatToElemental(0.0f, EFFECT_POISON);
						break;
					case CurrentElemntalStat::FIRE:
						gChar->GridCharReatToElemental(0.0f, EFFECT_BURN);
						break;
					case CurrentElemntalStat::FROZEN:
						gChar->GridCharReatToElemental(0.0f, EFFECT_FREEZE);
						break;
					case CurrentElemntalStat::PARALYSIS:
						gChar->GridCharReatToElemental(0.0f, EFFECT_PARALYSIS);
						break;
					}
				}
			}
			else
			{
				//Elementals should propagate their effects to other elementals
				AElementalHazard* elem = Cast<AElementalHazard>(OtherActor);
				if (elem)
				{
					switch (currentStat)
					{
					case CurrentElemntalStat::POISON:
						elem->ObstacleTakeDamage(10.0f, EFFECT_POISON);
						break;
					case CurrentElemntalStat::FIRE:
					case CurrentElemntalStat::EXPLOSION:
						elem->ObstacleTakeDamage(10.0f, EFFECT_BURN);
						break;
					case CurrentElemntalStat::FROZEN:
						elem->ObstacleTakeDamage(10.0f, EFFECT_FREEZE);
						break;
					case CurrentElemntalStat::PARALYSIS:
						elem->ObstacleTakeDamage(10.0f, EFFECT_PARALYSIS);
						break;
					}
				}
			}
		}
	}
}

void AElementalHazard::ATurnHasPassed(int turnType_)
{
	//Called by the obstacle manager when a battle phase passes
	//Check every turn that passes since you last changed. If it's the same turn that you changed state in, then count down the amount of turns until you disappear
	if (turnThatStatChangeOccuredIn == turnType_)
	{
		turnsSinceStateChanged--;
		if (turnsSinceStateChanged <= 0)
		{
			turnsSinceStateChanged = 0;
			currentStat = CurrentElemntalStat::NONE;
			DelayedDestroy();
		}
	}
}

CurrentElemntalStat AElementalHazard::GetCurrentState()
{
	return currentStat;
}

void AElementalHazard::ActivateOutline(bool value_)
{
	if (bCanBeTargeted)
	{
		if (value_)
		{
			staticMesh->SetCustomDepthStencilValue(6);
			box->SetCollisionProfileName("DestructibleBox"); //The collision should be enabled only when targeting as not to affect movement
		}
		else
		{
			staticMesh->SetCustomDepthStencilValue(0);
			box->SetCollisionProfileName("OverlapAllDynamic");
		}
	}
}

void AElementalHazard::AddObstacleToObstacleManager()
{
	Super::AddObstacleToObstacleManager();
	box->SetCollisionProfileName("OverlapAllDynamic");
}