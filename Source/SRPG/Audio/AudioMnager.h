// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AudioMnager.generated.h"

UCLASS()
class SRPG_API AAudioMnager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAudioMnager();

protected:
	void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Root")
		USceneComponent* root;
	UPROPERTY(EditAnywhere, Category = "AudioComponent")
		class UAudioComponent* audioComponent;
	UPROPERTY(EditAnywhere, Category = "SoundEffectsAudioComponent")
		class UAudioComponent* soundEffectAudioComponent;
	UPROPERTY(EditAnywhere, Category = "AudioComponent")
		TArray<class USoundBase*> music;
	UPROPERTY(EditAnywhere, Category = "SoundEffectsAudioComponent")
		TArray<USoundBase*> soundEffects;
	UPROPERTY(EditAnywhere, Category = "AudioComponent")
		int musicIndex;
	UPROPERTY(EditAnywhere, Category = "AudioComponent")
		float musicVolume;

	FTimerHandle fadeOutTimeHandle;
	float fadeOutTime;

public:
	void SwitchMusic(int musicIndex_);
	void PlayNextPiece();
	void FadeOutCurrentAudio();
	void PlayCommonSoundEffect(int index_); //No fading out

};
