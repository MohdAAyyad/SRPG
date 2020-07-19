// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePawn.h"

#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/DecalComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "BattleController.h"
#include "Engine/World.h"

// Sets default values
ABattlePawn::ABattlePawn()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	cameraBoom->SetupAttachment(root);
	cameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	cameraBoom->TargetArmLength = 800.f;
	cameraBoom->RelativeRotation = FRotator(-90.f, 0.f, 0.f);
	cameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	mainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	mainCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
	mainCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	bUnderControl = true;

}

// Called when the game starts or when spawned
void ABattlePawn::BeginPlay()
{
	Super::BeginPlay();

	if (CursorToWorld != nullptr)
	{
		pController = Cast<ABattleController>(GetController());
		if (pController)
		{
			FHitResult TraceHitResult;
			pController->GetHitResultUnderCursor(ECC_Camera, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);

			pController->SetBattlePawn(this);
		}
	}

}

// Called every frame
void ABattlePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABattlePawn::MoveUpDown(float rate_)
{
	if (bUnderControl)
	{
		FVector loc = GetActorLocation();
		loc.X += rate_ * 300.0f * GetWorld()->DeltaTimeSeconds;
		SetActorLocation(loc);
	}
}
void ABattlePawn::MoveRightLeft(float rate_)
{
	if (bUnderControl)
	{
		FVector loc = GetActorLocation();
		loc.Y += rate_ * 300.0f * GetWorld()->DeltaTimeSeconds;
		SetActorLocation(loc);
	}
}

void ABattlePawn::SetUnderControl(bool value_)
{
	bUnderControl = value_;
}

void ABattlePawn::Zoom(float rate_)
{
	if (bUnderControl)
	{
		mainCamera->SetFieldOfView(mainCamera->FieldOfView + rate_ * 30.0f * GetWorld()->DeltaTimeSeconds);
	}
}

