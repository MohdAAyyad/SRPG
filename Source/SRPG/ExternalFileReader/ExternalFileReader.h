// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueTableStruct.h"
#include "FighterTableStruct.h"
#include "Engine/DataTable.h"
#include "ExternalFileReader.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SRPG_API UExternalFileReader : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
		TArray<UDataTable*> tables;
	// is this the first time running
	bool firstTime;
	// index 0 = dialogue table

public:	
	// Sets default values for this component's properties
	UExternalFileReader();

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// finds a particular row if you know it's name 
	FDialogueTableStruct FindDialogueTableRow(FName name_, int index_);
	FFighterTableStruct FindFighterTableRow(FName name_, int index_);
	void AddRowToFighterTable(FName rowName_, int index_, FFighterTableStruct row_);
	//gets a ref to this external file reader
	UExternalFileReader* GetExternalFileReader();

	UDataTable* GetTable(int index_);


		
};
