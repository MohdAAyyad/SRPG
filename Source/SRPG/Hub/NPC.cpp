// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"
#include "SRPGCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/MeshComponent.h"
// Sets default values
ANPC::ANPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = root;


	startDialogueBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	startDialogueBox->SetupAttachment(RootComponent);
	//startDialogueBox->SetCollisionProfileName("BlockAllDynamic");
	startDialogueBox->OnComponentBeginOverlap.AddDynamic(this, &ANPC::OnOverlapWithPlayer);
	//startDialogueBox->OnComponentEndOverlap.AddDynamic(this, &ANPC::UnInteract);
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	meshComp->SetupAttachment(startDialogueBox);
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();

	
}

void ANPC::OnOverlapWithPlayer(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (interactedWith == true)
	{
		ASRPGCharacter* player = Cast<ASRPGCharacter>(OtherActor);
		if (OtherActor)
		{
			TestPrint();
		}
	}
	
}

void ANPC::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (meshSkeletal)
	{
		meshComp->SetSkeletalMesh(meshSkeletal);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("mesh Skeletal not initialised"));
	}

}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
//void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	//Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

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

