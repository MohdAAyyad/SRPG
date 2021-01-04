// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"
#include "SRPGCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "SRPGCharacter.h"
#include "Engine/Engine.h"
#include "Hub/HubWorldManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Hub/NPCs/NPCWanderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/NPCCharacterAnimInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ANPC::ANPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	//RootComponent = root;
	SetRootComponent(GetCapsuleComponent());
	// create the dialogue box and setup it's attributes 
	startDialogueBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	startDialogueBox->SetupAttachment(RootComponent);
	startDialogueBox->SetCollisionProfileName("NPCDialogue");
	// same thing for mesh component
	/*meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	meshComp->SetCollisionProfileName("Pawn");*/


	//create the file reader
	fileReader = CreateDefaultSubobject<UExternalFileReader>(TEXT("File Reader"));
	// create the widget component
	widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	/*widget->SetupAttachment(GetCapsuleComponent());
	widget->SetVisibility(false);*/
	// create the wander component
	wander = CreateDefaultSubobject<UNPCWanderComponent>(TEXT("Wander Component"));
	// set up the character movement 
	// set this to true to rotate the character to the movement vector
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.f, 150.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->GravityScale = 1.0f;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = false;
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->AddRelativeLocation(FVector(-10.f, 0.0f, 20.0f));
	CameraBoom->RelativeRotation = FRotator::ZeroRotator;
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	TopDownCameraComponent->AddRelativeLocation(FVector(-360.0f, 0.0f, 0.0f));
	TopDownCameraComponent->SetActive(false);
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
	startDialogueBox->OnComponentBeginOverlap.AddDynamic(this, &ANPC::OnOverlapWithPlayer);
	// load a default conversation if nothing is selected
	npcLineIndex = 0;


	
	animator = Cast<UNPCCharacterAnimInstance>(GetMesh()->GetAnimInstance());

	wander->SetCharacterRef(this);
	line = "";
	hasSetLine = false;

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
				playerRef = player;
				if (widget && widget->GetUserWidgetObject()->IsInViewport() == false)
				{
					widget->GetUserWidgetObject()->AddToViewport();
					LoadText();
				}
				
			}
		}
	}
}


void ANPC::LoadText()
{
	if (hasSetLine == false)
	{
		file = fileReader->FindDialogueTableRow(FName(*FString::FromInt(npcLineIndex)), 0);
		line = file.line;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Everything worked according to plan!"));


}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// handle the wander
	if (wander->GetShouldMove() && interactedWith == false && shouldWander)
	{
		wander->Wander();
	}

	if (wander->GetShouldMove() && animator)
	{
		if (interactedWith)
		{
			animator->SetIsWalking(false);
		}
		else
		{
			animator->SetIsWalking(true);
		}

	}
	else if(animator && wander->GetShouldMove() == false)
	{
		animator->SetIsWalking(false);
	}

	//	FQuat npcRot = GetActorRotation().Quaternion();
	//	FQuat playerRot = playerRef->GetActorRotation().Quaternion();
	//	FQuat result1 = playerRot.Inverse() * npcRot;
	//	//SetActorRotation(playerRot.Inverse() * npcRot);
	//	FQuat result2 = playerRot * npcRot;
	//*	npcRot = GetActorRotation().Quaternion();
	//	playerRot = playerRef->GetActorRotation().Quaternion();*/

	//	//SetActorRotation(playerRot * npcRot);
	//	result1 *= result2;
	//	FVector newRotation = result1.Vector() + FVector(0, 0, 180);
	//	//SetActorRotation(FQuat::MakeFromEuler(newRotation));

	//	FVector interp = FMath::Lerp(newRotation, GetActorRotation().Vector(), 1);
	//	UE_LOG(LogTemp, Warning, TEXT("new target = "));
	//	FRotator rot = FRotator(0, 0, interp.Z);
	//	SetActorRotation(rot);

	// rotate the NPC to face the player
	if (playerRef && interactedWith)
	{
		FVector npcRot = GetActorLocation();
		FVector playerRot = playerRef->GetActorLocation();

		FRotator result = UKismetMathLibrary::FindLookAtRotation(npcRot, playerRot);

		FRotator distance = GetActorRotation() - result;
		if (GetActorRotation().Yaw > result.Yaw + 2)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Rotating Negative!"));
			FRotator rot = FRotator(0, -400 * DeltaTime, 0);
			AddActorLocalRotation(rot);
		}
		else if (GetActorRotation().Yaw < result.Yaw - 2)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Rotating Positive!"));
			FRotator rot = FRotator(0, 400 * DeltaTime, 0);
			AddActorLocalRotation(rot);
		}
	}	
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

void ANPC::SetLine(FString line_)
{
	line = line_;
}

void ANPC::SetNPCLinesIndex(int index_)
{
	npcLineIndex = index_;
	hasSetLine = true;
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
	//UE_LOG(LogTemp, Warning, TEXT("UnInteracted!"));
	if (widget)
	{
		widget->GetUserWidgetObject()->RemoveFromViewport();
	}
}

void ANPC::DelayedAddWidgetToViewPort()
{
	//Named "Delayed" for no reason other than the fact this is called by the shops/NPCs when they back the camera out

	//By now the camera will have moved to focus on the item shop
	if (widget)
		if (widget->GetUserWidgetObject())
			widget->GetUserWidgetObject()->AddToViewport();
}


AHubWorldManager* ANPC::GetHubWorldManager()
{
	return hubManager;
}

