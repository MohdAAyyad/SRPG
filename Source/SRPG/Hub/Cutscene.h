// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "Cutscene.generated.h"

UCLASS()
class SRPG_API ACutscene : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACutscene();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Root")
	USceneComponent* root;


	// need to figure out how to display the models at set locations relative to this object
	class ACutsceneActor* model1;
	class ACutsceneActor* model2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Widget")
	class UWidgetComponent* widget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File Reader")
	class UExternalFileReader* fileReader;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int currentDialogue;

	void LoadInfo();

	FCutsceneTableStruct currentLine;

	UPROPERTY(BlueprintReadOnly)
	FString dialogue;
	UPROPERTY(BlueprintReadOnly)
	FString name;
	UPROPERTY(BlueprintReadOnly)
	float textSpeed;
	UPROPERTY(BlueprintReadOnly)
	FString model1Name;
	UPROPERTY(BlueprintReadOnly)
	FString model2Name;
	UPROPERTY(BlueprintReadOnly)
	int lineNum;


	//tells the UI that the cutscene is over
	UPROPERTY(BlueprintReadOnly)
	bool done;

	void SpawnActors();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector offset1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector offset2;

	UPROPERTY(BlueprintReadOnly)
	FString modelAnim1;

	UPROPERTY(BlueprintReadOnly)
	FString modelAnim2;

	FString prevAnim1;
	FString prevAnim2;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UFUNCTION(BlueprintCallable)
	void ShowUI();

	UFUNCTION(BlueprintCallable)
	void StartCutscene();

	UFUNCTION(BlueprintCallable)
	void AdvanceDialogue();

	UFUNCTION(BlueprintCallable)
	void LoadModels();

	UFUNCTION(BlueprintCallable)
	void LoadAnimations();

	UFUNCTION(BlueprintCallable)
	void EndCutscene();

	UFUNCTION(BlueprintCallable)
		void CheckDone();




	// list of all the possible models that can be loaded in
	UPROPERTY(EditAnywhere)
	TArray<class USkeletalMesh*> models;
	// all the anims that can be played
	UPROPERTY(EditAnywhere)
	TArray<UAnimationAsset*> anims;

	UPROPERTY(EditAnywhere)
	float fadeTime;
	

};
