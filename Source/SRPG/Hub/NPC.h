// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interactable.h"
#include "Animation/NPCCharacterAnimInstance.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "NPC.generated.h"

UCLASS()
class SRPG_API ANPC : public ACharacter, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// used for dialogue

	// box component used to start dialogue with the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* startDialogueBox;
	// determines weather or not this NPC is currently engaged in a conversation
	bool interactedWith;
	//which opponent are they talking about
	int talkingAboutOpponent;
	// 0 tournament, 1 story, 2 random, determined by the hub manager after spawning them
	int npcLineIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshComponent")
	class USkeletalMeshComponent* meshComp;

	// root of the scene
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root")
	class USceneComponent* root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "File Reader")
	class UExternalFileReader* fileReader;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Widget")
	class UWidgetComponent* widget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wander")
	class UNPCWanderComponent* wander;

	UNPCCharacterAnimInstance* animator;

	UFUNCTION()
	virtual void OnOverlapWithPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
									 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								     bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		virtual void LoadText();

	UFUNCTION(BlueprintCallable)
		virtual void LeaveNPC() {}; //Overriden in children
	// gives us a ref to the hubmanager for time managment purposes
	//UPROPERTY(EditAnywhere, Category = "HubManager")
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class AHubWorldManager* hubManager;

	class ASRPGCharacter* playerRef;
	// ref to the player once collided, can be used in blueprints for UI purposes
	UPROPERTY(EditAnywhere)
	bool shouldWander;


	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// overrides of the interface functions
	void Interact() override;
	void UnInteract() override;

	void SetLine(FString line_);
	void SetNPCLinesIndex(int index_);
	void SetHubManager(class AHubWorldManager* manager_);

	UFUNCTION(BlueprintCallable)
	virtual void EndDialogue(); // overriden by children to end the dialogue

	void DelayedAddWidgetToViewPort();

	UFUNCTION(BlueprintCallable)
	AHubWorldManager* GetHubWorldManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString line;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool skippable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float textSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name;

private:
	FDialogueTableStruct file;



	bool hasSetLine;

};
