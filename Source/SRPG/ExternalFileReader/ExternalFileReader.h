// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueTableStruct.h"
#include "FighterTableStruct.h"
#include "ItemTableStruct.h"
#include "EquipmentTableStruct.h"
#include "ActivityDialogueTableStruct.h"
#include "FSkillTableStruct.h"
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
	FActivityDialogueTableStruct FindActivityDialogueTableRow(FName name_, int index_);
	FActivityDialogueTableStruct GetPositiveBranch(int activityIndex_, int tableIndex_);
	FActivityDialogueTableStruct GetNegativeBranch(int activityIndex_, int tableIndex_);
	FActivityDialogueTableStruct GetPositiveCentral(int activityIndex_, int tableIndex_);
	FActivityDialogueTableStruct GetNegativeCentral(int activityIndex_, int tableIndex_);
	TArray<FSkillTableStruct*> GetOffesniveSkills(int weaponIndex_, int currentLevel_);
	TArray<FSkillTableStruct*> GetDefensiveSkills(int armodIndex_, int currentLevel_);
	TArray<int> GetWeaponSpeeds(int weaponID_);
	TArray<FFighterTableStruct*> GetAllRecruitedFighters();
	FFighterTableStruct GetRecruitedFighterByID(int id_);
	TArray<FItemTableStruct> GetAllOwnedItems();
	int GetItemStatIndex(FName itemName_);
	int GetItemValue(FName itemName_);
	void AddRowToFighterTable(FName rowName_, int index_, FFighterTableStruct row_);
	void AddOwnedValueItemTable(FName rowName_, int index_, int value_);
	void AddOwnedValueEquipmentTable(FName rowName_, int index_, int value_);
	FName ConvertItemNameToNameUsedInTable(FName name_);
	


	//gets a ref to this external file reader
	UExternalFileReader* GetExternalFileReader();

	UDataTable* GetTable(int index_);


		
};
