// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"
#include "SRPGCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
// Sets default values
ANPC::ANPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = root;


	startDialogueBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	startDialogueBox->SetupAttachment(root);
	startDialogueBox->SetCollisionProfileName("NPCDialogue");
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	meshComp->SetCollisionProfileName("Pawn");
	meshComp->SetupAttachment(root);
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
	startDialogueBox->OnComponentBeginOverlap.AddDynamic(this, &ANPC::OnOverlapWithPlayer);
	
}

void ANPC::OnOverlapWithPlayer(UPrimitiveComponent * overlappedComp_, AActor * otherActor_, 
						       UPrimitiveComponent * otherComp_, int32 otherBodyIndex_, 
							bool bFromSweepO, const FHitResult & sweepResult_)
{
	if (otherActor_ != nullptr && otherActor_ != this && overlappedComp_ != nullptr)
	{
		if (interactedWith == true)
		{
			ASRPGCharacter* player = Cast<ASRPGCharacter>(otherActor_);
			if (otherActor_)
			{
				TestPrint();
				interactedWith = false;
			}
		}
	}
	
}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANPC::Interact()
{
	interactedWith = true;
	UE_LOG(LogTemp, Warning, TEXT("Interacted!"));
}

void ANPC::UnInteract()
{
	interactedWith = false;
	UE_LOG(LogTemp, Warning, TEXT("UnInteracted!"));
}

void ANPC::TestPrint()
{
	UE_LOG(LogTemp, Warning, TEXT("Everything worked according to plan!"));
}

void ANPC::SetNPCLinesIndex(int index_)
{
	if (index_ <= 2 && index_ >= 0)
	{
		npcLineIndex = index_;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Input for NPCLineIndex"));
	}

}

void ANPC::EndDialogue()
{
	// 
}

