// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdItem.h"
#include "Battle/BattleManager.h"
#include "BattleCrowd.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "../GridCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "../Grid/Tile.h"
#include "../Player/PlayerGridCharacter.h"
#include "../AI/EnemyBaseGridCharacter.h"
#include "CollisionQueryParams.h"

// Sets default values
ACrowdItem::ACrowdItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = root;

	sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	sphere->SetGenerateOverlapEvents(true);
	sphere->SetCollisionProfileName("OverlapAll");
	sphere->SetupAttachment(root);

	particles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	particles->SetupAttachment(root);
	markedByThisEnemy = nullptr;
	myTile = nullptr;
}

// Called when the game starts or when spawned
void ACrowdItem::BeginPlay()
{
	Super::BeginPlay();

	if (sphere)
	{
		sphere->OnComponentBeginOverlap.AddDynamic(this, &ACrowdItem::OverlapWithGridCharacter);
	}

	if (spawnParticles)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), spawnParticles, GetActorLocation(), GetActorRotation());

	GetMyTile();
	
}

// Called every frame
void ACrowdItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACrowdItem::SetBtlAndCrdManagers(ABattleManager* btl_, ABattleCrowd* crd_)
{
	btlManager = btl_;
	crdManager = crd_;

	if (btlManager)
		turnsElapsedSinceSpawned = btlManager->GetTotalNumberOfPhasesElapsed();
}
void ACrowdItem::CheckIfItemShouldBeDestroyed()
{
	if (bMarkedByEnemy)
		bMarkedByEnemy = false;
	if (btlManager)
	{
		if (btlManager->GetTotalNumberOfPhasesElapsed() - turnsElapsedSinceSpawned >= turnsUntilItemIsDestroyed)
		{
			if (crdManager)
			{
				crdManager->RemoveSpawnedItem(this);
				if (endParticles)
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), endParticles, GetActorLocation(), GetActorRotation());

				FTimerHandle endHandle;
				GetWorld()->GetTimerManager().SetTimer(endHandle,this, &ACrowdItem::DespawnItem,0.15f, false);
				
			}
		}
	}
}
void ACrowdItem::OverlapWithGridCharacter(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_,
	bool bFromSweep_,
	const FHitResult &sweepResult_)
{
	if (otherActor_ != nullptr && otherActor_ != this && otherComp_ != nullptr)
	{
		//The reason we're checking for playergridcharacter and not gridcharacter is because enemies use collisions to detect the items and if we used gridcharacter, enemies will take the itmes when merely detecting them
		APlayerGridCharacter* gchar = Cast<APlayerGridCharacter>(otherActor_);
		if (gchar)
		{
			//TODO
			//Call the needed functions on gchar

			Obtained(gchar->GetActorLocation());
			
		}
	}
}

void ACrowdItem::DespawnItem()
{
	Destroy();
}

bool ACrowdItem::MarkItem(AEnemyBaseGridCharacter* markingEnemy_) //Called by enemies to make sure no one else takes this item. Return true if operation was a success
{
	if (!bMarkedByEnemy)
	{
		markedByThisEnemy = markingEnemy_;
		bMarkedByEnemy = true;
		return true;
	}
	return false;
}
int ACrowdItem::GetStat()
{
	return statIndex;
}
int ACrowdItem::GetValue()
{
	return value;
}

ATile* ACrowdItem::GetMyTile()
{
	if (!myTile)
	{
		FHitResult hit;
		FVector end = GetActorLocation();
		end.Z -= 400.0f;
		FCollisionQueryParams queryParms(FName(TEXT("query")), false, this);
		queryParms.AddIgnoredActor(this);
		if (GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), end, ECollisionChannel::ECC_Visibility, queryParms))
		{
			myTile = Cast<ATile>(hit.Actor);
		}
	}

	return myTile;
}

void ACrowdItem::Obtained(FVector loc_)
{
	if (obtainedParticles)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), obtainedParticles, loc_, GetActorRotation());

	if (crdManager)
		crdManager->RemoveSpawnedItem(this);
}

void ACrowdItem::ItemWasObtainedByAnEnemyThatDidNotMarkIt()
{
	if (markedByThisEnemy)
		markedByThisEnemy->ItemIsUnreachable(nullptr);
}

