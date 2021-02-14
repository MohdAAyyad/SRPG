// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioMnager.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "../TimerManager.h"

// Sets default values
AAudioMnager::AAudioMnager()
{
	PrimaryActorTick.bCanEverTick = false;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = root;

	audioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	audioComponent->SetupAttachment(root);

	soundEffectAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Sound Effects Audio Component"));
	soundEffectAudioComponent->SetupAttachment(root);

	
	musicIndex = 0;
	fadeOutTime = 2.5f;
	musicVolume = 0.8f;
}

void AAudioMnager::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle postInitHandle;
	GetWorld()->GetTimerManager().SetTimer(postInitHandle, this, &AAudioMnager::PostInit, 0.3f, false);
}

void AAudioMnager::PostInit()
{
	for (int i = 0; i < music.Num(); i++)
	{
		//Load all the sound files at the beginning to prevent FPS drops when they're loaded
		audioComponent->VolumeMultiplier = 0.0f;
		audioComponent->Sound = music[i];
		audioComponent->Stop();
		audioComponent->Play();
	}
	audioComponent->VolumeMultiplier = musicVolume;

	if (musicIndex >= 0 && musicIndex < music.Num())
	{
		if (music[musicIndex])
		{
			PlayNextPiece();
		}
	}
}

void AAudioMnager::SwitchMusic(int musicIndex_)
{
	if (musicIndex_ != musicIndex) //Don't switch if it's the same track
	{
		if (music[musicIndex_])
		{
			musicIndex = musicIndex_;
			if (audioComponent->IsPlaying()) //If playing fade out first
			{
				audioComponent->FadeOut(fadeOutTime, 0.0f);
				GetWorld()->GetTimerManager().SetTimer(fadeOutTimeHandle, this, &AAudioMnager::PlayNextPiece, fadeOutTime, false);
			}
			else //Otherwise, start playing
			{
				PlayNextPiece();
			}

		}
	}
}
void AAudioMnager::PlayNextPiece()
{
	if (musicIndex >= 0 && musicIndex < music.Num())
	{
		if (music[musicIndex])
		{
			audioComponent->Sound = music[musicIndex];
			//audioComponent->Play();
			audioComponent->FadeIn(8.0f, musicVolume);
		}
	}
}

void AAudioMnager::FadeOutCurrentAudio()
{
	audioComponent->FadeOut(1.2f, 0.0f);
}

void AAudioMnager::PlayCommonSoundEffect(int index_) //No fading out
{
	if (index_ >= 0 && index_ < soundEffects.Num())
	{
		if (soundEffects[index_])
		{
			soundEffectAudioComponent->Sound = soundEffects[index_];
			soundEffectAudioComponent->Play();
		}
	}
}
