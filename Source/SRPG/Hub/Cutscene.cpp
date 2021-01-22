// Fill out your copyright notice in the Description page of Project Settings.


#include "Cutscene.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "CutsceneActor.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SRPGPlayerController.h"
#include "SRPGCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACutscene::ACutscene()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	// set up cameras
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = false;
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->AddRelativeLocation(FVector(-10.f, 0.0f, 20.0f));
	CameraBoom->RelativeRotation = FRotator::ZeroRotator;
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;
	TopDownCameraComponent->AddRelativeLocation(FVector(-360.0f, 0.0f, 0.0f));
	TopDownCameraComponent->SetActive(false);

	// set up the file reader and UI 
	fileReader = CreateDefaultSubobject<UExternalFileReader>(TEXT("File Reader"));
	widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));



}

// Called when the game starts or when spawned
void ACutscene::BeginPlay()
{
	Super::BeginPlay();

	// set values to default
	currentDialogue = 0;
	done = false;
	prevAnim1 = "";
	prevAnim2 = "";

	//StartCutscene();
}

void ACutscene::LoadInfo()
{
	// im making it always search 0 since it will always only have one single table
	// load in all the info about this segment
	FCutsceneTableStruct row = fileReader->FindCutsceneTableRow(FName(*FString::FromInt(currentDialogue)), 0);


	// set all the data
	dialogue = row.dialogue;
	textSpeed = row.textSpeed;
	name = row.name;
	model1Name = row.model1Name;
	model2Name = row.model2Name;
	lineNum = row.lineNum;
	modelAnim1 = row.modelAnim1;
	modelAnim2 = row.modelAnim2;
}

void ACutscene::SpawnActors()
{
	FVector spawnLoc1 = GetActorLocation() + offset1;
	FVector spawnLoc2 = GetActorLocation() + offset2;

	model1 = GetWorld()->SpawnActor<ACutsceneActor>(spawnLoc1, GetActorRotation());
	model2 = GetWorld()->SpawnActor<ACutsceneActor>(spawnLoc2, GetActorRotation());

	// set the rotation of the second model so it faces the first
	FRotator rot = FRotator(0, 180, 0);
	model2->SetActorRotation(rot);
}

// Called every frame
void ACutscene::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACutscene::ShowUI()
{
	 
	if (widget && widget->GetUserWidgetObject()->IsInViewport() == false)
	{
		widget->GetUserWidgetObject()->AddToViewport();
	}
}

void ACutscene::StartCutscene()
{


	// keep it on zero so no magnifying
	// first thing that needs to happen is we need to load in the data
	// we will create a function for that
	APlayerCameraManager* cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (cam)
	{
		cam->StartCameraFade(1.0f, 0.0f, fadeTime, FLinearColor(0.0f, 0.0f, 0.0f), false, false);
	}




	LoadInfo();
	//spawn the actors
	SpawnActors();

	// next we need to load up the models relevant to the scene
	LoadModels();

	LoadAnimations();

	ShowUI();

	// grab a reference to the player
	ASRPGPlayerController* control = Cast<ASRPGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (control)
	{
		// basically snap to this npc, in the future i'll work on a camera fade
		control->FocusOnThisNPC(this, 0.001f);
		control->SetInputMode(FInputModeUIOnly());
	}
}

void ACutscene::AdvanceDialogue()
{
	// check to see if we are at the last line
	FCutsceneTableStruct row = fileReader->FindCutsceneTableRow(FName(*FString::FromInt(currentDialogue)), 0);
	if (currentDialogue >= row.lineNum)
	{
		done = true;

	}
	else
	{
		// called from UI to advance to the next dialogue
		currentDialogue++;

		LoadInfo();

		LoadModels();

		LoadAnimations();
	}


}

void ACutscene::LoadModels()
{

	if (model1 && model2)
	{
		int modelNum1 = 0;
		int modelNum2 = 0;

		// model 1 list
		if (model1Name == "Test1")
		{
			modelNum1 = 0;

		}
		else if (model1Name == "Test2")
		{
			modelNum1 = 1;
		}


		if (model2Name == "Test1")
		{
			modelNum2 = 0;
		}
		else if (model2Name == "Test2")
		{
			modelNum2 = 1;
		}

		//change up the actual models
		model1->SetMesh(models[modelNum2]);
		model2->SetMesh(models[modelNum2]);
	}
	

}

void ACutscene::LoadAnimations()
{
	int animNum1 = 0;
	int animNum2 = 0;
	bool play1 = false;
	bool play2 = false;

	if (model1 && model2)
	{
		// model 1 
		if (modelAnim1 == "Idle" && modelAnim1 != prevAnim1)
		{
			// we only wanna play the animation if it's actually changed
			// if it hasn't keep it the same
			animNum1 = 0;
			play1 = true;
		}
		else if (modelAnim1 == "Walking" && modelAnim1 != prevAnim1)
		{
			animNum1 = 1;
			play1 = true;
		}

		// model 2
		if (modelAnim2 == "Idle" && modelAnim2 != prevAnim2)
		{
			animNum2 = 0;
			play2 = true;
		}
		else if (modelAnim2 == "Walking" && modelAnim2 != prevAnim2)
		{
			animNum2 = 1;
			play2 = true;
		}
	}

	if(play1)
	{
		model1->PlayAnimation(anims[animNum1]);
		// get the new previous animation tag
		prevAnim1 = modelAnim1;
	}
	if (play2)
	{
		model2->PlayAnimation(anims[animNum2]);
		// get the new previous animation tag
		prevAnim2 = modelAnim2;
	}

}

void ACutscene::EndCutscene()
{
	ASRPGPlayerController* control = Cast<ASRPGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (control)
	{
		
		// snap back to reality
		control->FocusOnThisNPC(control->GetPlayerReference(), 0.001f);
		control->SetInputMode(FInputModeGameAndUI());

		APlayerCameraManager* cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		if (cam)
		{
			cam->StartCameraFade(1.0f, 0.0f, fadeTime, FLinearColor(0.0f, 0.0f, 0.0f), false, false);
		}
	}

	// remove any pesky ui elements left
	if (widget->GetUserWidgetObject()->IsInViewport())
	{
		widget->GetUserWidgetObject()->RemoveFromViewport();
	}

	// go and delete the actors
	model1->DeleteActor();
	model2->DeleteActor();

	model1 = nullptr;
	model2 = nullptr;

	
}



















































































































































// oop there goes gravity