// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interactable.h"
#include "NPC.generated.h"

UCLASS()
class SRPG_API ANPC : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// used for dialogue
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Line")
	FString line;
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

	UFUNCTION()
	virtual void OnOverlapWithPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
									 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								     bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		virtual void LoadText();
	// gives us a ref to the hubmanager for time managment purposes
	class AHubWorldManager* hubManager;
	// ref to the player once collided, can be used in blueprints for UI purposes
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// overrides of the interface functions
	void Interact() override;
	void UnInteract() override;
	void TestPrint();

	void SetNPCLinesIndex(int index_);
	void SetHubManager(class AHubWorldManager* manager_);
	virtual void EndDialogue(); // overriden by children to end the dialogue 

};
