// Fill out your copyright notice in the Description page of Project Settings.


#include "SRPGGameState.h"
#include "Net/UnrealNetwork.h"


ASRPGGameState::ASRPGGameState()
{
	// make sure that the game state is replicated
	SetReplicates(true);

}

void ASRPGGameState::OnRep_Test()
{
	UE_LOG(LogTemp, Error, TEXT("Test Value = %d"), test);
}

void ASRPGGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);



	DOREPLIFETIME(ASRPGGameState, test);
	DOREPLIFETIME(ASRPGGameState, test2);


}

void ASRPGGameState::BeginPlay()
{
	Super::BeginPlay();
	// has authority means if the client running the code is the server
	if (HasAuthority())
	{
		test = 20;
		OnRep_Test();
	}
}
