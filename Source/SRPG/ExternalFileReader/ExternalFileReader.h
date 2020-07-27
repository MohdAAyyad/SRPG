// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueTableStruct.h"
#include "FighterTableStruct.h"
#include "ItemTableStruct.h"
#include "EquipmentTableStruct.h"
#include "ExternalFileReader.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SRPG_API UExternalFileReader : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
		TArray<UDataTable*> tables;
	// is this the first time running for fighters
	bool firstTimeFighter;
	// is this the first time running for items
	bool firstTimeItem;
	// first time running equipment
	bool firstTimeEquipment;
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
	FItemTableStruct FindItemTableRow(FName name_, int index_);
	FEquipmentTableStruct FindEquipmentTableRow(FName name_, int index_);
	void AddRowToFighterTable(FName rowName_, int index_, FFighterTableStruct row_);
	void AddOwnedValueItemTable(FName rowName_, int index_, int value_);
	void AddOwnedValueEquipmentTable(FName rowName_, int index_, int value_);


	//gets a ref to this external file reader
	UExternalFileReader* GetExternalFileReader();

	UDataTable* GetTable(int index_);


		
};
