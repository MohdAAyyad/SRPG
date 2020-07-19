// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GridCharacter.generated.h"

UCLASS()
class SRPG_API AGridCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGridCharacter();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

protected:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
		class UWidgetComponent* widgetComp;


	bool bMoving;
	TArray<FVector> movementPath;
	class ATile* originTile;
	UPROPERTY(EditAnywhere, Category = "Grid")
		class UPathComponent* pathComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveAccordingToPath();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Selected();
	virtual void NotSelected();
	void UpdateOriginTile(); //Called at the beginning of every player turn
	void MoveToThisTile(ATile* target_);

};
