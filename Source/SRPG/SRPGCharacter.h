// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Hub/Interactable.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "SRPGCharacter.generated.h"

UCLASS(Blueprintable)
class ASRPGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASRPGCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	void SetInteracting(IInteractable* ref_);

	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable, WithValidation)
	void SetupController();
	void SetupController_Implementation();
	bool SetupController_Validate();

	void TriggerPauseMenu();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class UWidgetComponent* widget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File Reader")
	UExternalFileReader* fileReader;

	UPROPERTY(EditAnywhere)
	TArray<UTexture*> itemTextures;

	UPROPERTY(EditAnywhere)
	TArray<UTexture*> fighterTextures;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	
	//what thing are we interacting with
	IInteractable* interacting;

	UFUNCTION(BlueprintCallable)
	TArray<FEquipmentTableStruct> GetAllOwnedEquipment(FString tableName_);
	UFUNCTION(BlueprintCallable)
	TArray<FFighterTableStruct> GetAllFighters(FString tableName_);
	UFUNCTION(BlueprintCallable)
	TArray<UTexture*> GetAllItemTextures();
	UFUNCTION(BlueprintCallable)
	TArray<UTexture*> GetAllFighterTextures();
	UFUNCTION(BlueprintCallable)
	bool CanFighterEquipWeapon(FString tableName_, int id_);
	UFUNCTION(BlueprintCallable)
	bool CanFighterEquipArmor(FString tableName_, int id_);
	UFUNCTION(BlueprintCallable)
	bool CanFighterEquipAccessory(FString tableName_, int id_);

};

