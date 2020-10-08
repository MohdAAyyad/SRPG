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
	FFighterTableStruct FindEquippedFighterTableRow(FName name_, int index_);
	FActivityDialogueTableStruct FindActivityDialogueTableRow(FName name_, int index_);
	FActivityDialogueTableStruct GetPositiveBranch(int activityIndex_, int tableIndex_);
	FActivityDialogueTableStruct GetNegativeBranch(int activityIndex_, int tableIndex_);
	FActivityDialogueTableStruct GetPositiveCentral(int activityIndex_, int tableIndex_);
	FActivityDialogueTableStruct GetNegativeCentral(int activityIndex_, int tableIndex_);
	FActivityDialogueTableStruct GetStartingDialogue(int activityIndex_, int tableIndex_);
	FActivityDialogueTableStruct GetActivityEndDialogue(int activityIndex_, int tableIndex_);
	TArray<FSkillTableStruct*> GetOffesniveSkills(int tableIndex_, int weaponIndex_, int skillNum_, int skillsIndex_, int currentLevel_);
	TArray<FSkillTableStruct*> GetDefensiveSkills(int tableIndex_, int armorIndex_, int skillNum_, int skillsIndex_, int currentLevel_);
	FEquipmentTableStruct GetEquipmentById(int tableIndex_, int equipID_, int equipIndex_, int subIndex_);
	FEquipmentTableStruct GetEquipmentByLevel(int tableIndex_, int level_, int equipIndex_, int subIndex_);
	TArray<FItemTableStruct> GetAllItems(int tableIndex_, int worldLevel_);
	TArray<FEquipmentTableStruct> GetAllEquipment(int tableIndex_, int worldLevel_);
	TArray<FFighterTableStruct> GetAllRecruitedFighters(int tableIndex_);
	FFighterTableStruct GetRecruitedFighterByID(int id_);
	void RemoveFightersDueToPermaDeath(TArray<int>& ids_, int tableIndex_);
	TArray<FItemTableStruct> GetAllOwnedItems();
	int GetItemStatIndex(int tableIndex_, FName itemName_);
	int GetItemValue(FName itemName_);
	void AddRowToRecruitedFighterTable(FName rowName_, int index_, FFighterTableStruct row_);
	void AddOwnedValueItemTable(FName rowName_, int index_, int value_);
	void AddOwnedValueEquipmentTable(FName rowName_, int index_, int value_);
	//void ClearRecruitedFightersTable(int tableIndex_);
	void RemoveFighterTableRow(FName rowName_, int tableIndex_);
	FName ConvertItemNameToNameUsedInTable(FName name_);

	//gets a ref to this external file reader
	UExternalFileReader* GetExternalFileReader();

	UDataTable* GetTable(int index_);

	// the name must match the class exactly
	int FindTableIndexInArray(FName structName_);
	int FindTableIndexByName(FName tableName_);

//	void RemoveFighterTableRow(FName rowName_, int tableIndex_);
		
};
