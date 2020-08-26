// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"
#include "SRPGCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Components/WidgetComponent.h"
#include "SRPGCharacter.h"
#include "Engine/Engine.h"
#include "Hub/HubWorldManager.h"
// Sets default values
ANPC::ANPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = root;

	// create the dialogue box and setup it's attributes 
	startDialogueBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	startDialogueBox->SetupAttachment(root);
	startDialogueBox->SetCollisionProfileName("NPCDialogue");
	// same thing for mesh component
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	meshComp->SetCollisionProfileName("Pawn");
	meshComp->SetupAttachment(root);

	//create the file reader
	fileReader = CreateDefaultSubobject<UExternalFileReader>(TEXT("File Reader"));
	// create the widget component
	widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
	startDialogueBox->OnComponentBeginOverlap.AddDynamic(this, &ANPC::OnOverlapWithPlayer);
	// load a default conversation if nothing is selected
	npcLineIndex = 0;
	//UE_LOG(LogTemp, Warning, TEXT("Begin play on NPC"));
	if (idle && meshComp)
	{
		meshComp->SetAnimation(idle);
		meshComp->PlayAnimation(idle, true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Idle Animation is NULL"));
	}


}

void ANPC::OnOverlapWithPlayer(UPrimitiveComponent * overlappedComp_, AActor * otherActor_, 
						       UPrimitiveComponent * otherComp_, int32 otherBodyIndex_, 
							bool bFromSweepO, const FHitResult & sweepResult_)
{

	if (otherActor_ != nullptr && otherActor_ != this && overlappedComp_ != nullptr)
	{
		// check if we are being interacted with and process the logic 
		if (interactedWith)
		{
			ASRPGCharacter* player = Cast<ASRPGCharacter>(otherActor_);
			if (player)
			{
				if (widget)
				{
					widget->GetUserWidgetObject()->AddToViewport();
					UE_LOG(LogTemp, Warning, TEXT("Added Widget To viewport"));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Widget is NULL"));
				}
				LoadText();
			}
		}
	}
}


void ANPC::LoadText()
{
	UE_LOG(LogTemp, Warning, TEXT("Everything worked according to plan!"));
	FDialogueTableStruct file = fileReader->FindDialogueTableRow(FName(*FString::FromInt(npcLineIndex)), 0);

	line = file.line;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, line);
}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANPC::Interact()
{
	interactedWith = true;
	//UE_LOG(LogTemp, Warning, TEXT("Interacted!"));
}

void ANPC::UnInteract()
{
	interactedWith = false;
	EndDialogue();
}

void ANPC::SetNPCLinesIndex(int index_)
{
	npcLineIndex = index_;
}

void ANPC::SetHubManager(AHubWorldManager* manager_)
{
	if (manager_)
	{
		hubManager = manager_;
	}
}

void ANPC::EndDialogue()
{
	UE_LOG(LogTemp, Warning, TEXT("UnInteracted!"));
	if (widget)
	{
		widget->GetUserWidgetObject()->RemoveFromViewport();
	}
}


