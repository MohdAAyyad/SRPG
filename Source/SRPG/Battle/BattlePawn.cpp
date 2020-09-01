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
	cameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
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
	lockedOnTarget = nullptr;
	bLockedOn = false;
	lockOnRate = 0.075f;
	originalFOV = 0.0f;

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

	originalFOV = mainCamera->FieldOfView;

}

// Called every frame
void ABattlePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bLockedOn)
	{
		//Follow the locked on target in the x-y plane
		if (lockedOnTarget)
		{
			FVector loc = GetActorLocation();
			FVector targetLoc = lockedOnTarget->GetActorLocation();
			targetLoc.Z = loc.Z;
			loc = FMath::Lerp(loc, targetLoc, lockOnRate);
			if (mainCamera->FieldOfView != originalFOV)
				mainCamera->SetFieldOfView(FMath::Lerp(mainCamera->FieldOfView, originalFOV, lockOnRate));
			SetActorLocation(loc);
		}
	}

}

void ABattlePawn::MoveUpDown(float rate_)
{
	if (!bLockedOn)
	{
		FVector loc = GetActorLocation();
		loc.X += rate_ * 600.0f * GetWorld()->DeltaTimeSeconds;
		SetActorLocation(loc);
	}
}
void ABattlePawn::MoveRightLeft(float rate_)
{
	if (!bLockedOn)
	{
		FVector loc = GetActorLocation();
		loc.Y += rate_ * 600.0f * GetWorld()->DeltaTimeSeconds;
		SetActorLocation(loc);
	}
}


void ABattlePawn::Zoom(float rate_)
{
	if (bUnderControl)
	{
		if (FMath::Abs(rate_) > 0)
		{
			mainCamera->SetFieldOfView(mainCamera->FieldOfView + rate_ * 30.0f * GetWorld()->DeltaTimeSeconds);
		}
	}
}

void ABattlePawn::LockOnActor(AActor* target_)
{
	lockedOnTarget = target_;
	bLockedOn = true;
}

void ABattlePawn::ResetLock()
{
	bLockedOn = false;
	lockedOnTarget = nullptr;
}

