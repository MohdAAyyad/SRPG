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
#include "CutsceneTableStruct.h"
#include "FDayTableStruct.h"
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
	FCutsceneTableStruct FindCutsceneTableRow(FName name_, int index_);

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

	TArray<FSkillTableStruct> GetOffesniveSkillsForBP(int tableIndex_, int weaponIndex_, int skillNum_, int skillsIndex_, int currentLevel_);
	TArray<FSkillTableStruct> GetDefensiveSkillsForBP(int tableIndex_, int armorIndex_, int skillNum_, int skillsIndex_, int currentLevel_);


	TArray<FItemTableStruct> GetAllItemsConditionedByWorldLevel(int tableIndex_, int worldLevel_);
	TArray<FItemTableStruct> GetAllOwnedItems(int tableIndex_);
	int GetItemStatIndex(int tableIndex_, FName itemName_);
	int GetItemValue(FName itemName_);
	void AddRowToRecruitedFighterTable(FName rowName_, int index_, FFighterTableStruct row_);
	void AddOwnedValueItemTable(int tableIndex_, int itemId_, int amountToAdd_);
	void UpdateOwnedValueItemTableAfterLoad(int tableIndex, int itemID_, int newOwnedValue_);

	void AddOwnedValueEquipmentTable(FName rowName_, int index_, int value_);
	void UpdateOwnedValueEquipmentTableAfterLoad(int tableIndex, int equipID_, int newOwnedValue_);
	FEquipmentTableStruct GetEquipmentById(int tableIndex_, int equipID_, int equipIndex_, int subIndex_);
	FEquipmentTableStruct GetEquipmentByLevel(int tableIndex_, int level_, int equipIndex_, int subIndex_);
	TArray<FEquipmentTableStruct> FindAllOwnedEquipment(int tableIndex_);
	TArray<FEquipmentTableStruct> GetAllOwnedEquipmentOfACertainType(int tableIndex_, int equipIndex_, int subIndex_);
	TArray<FEquipmentTableStruct> GetAllEquipmentOfACertainTypeConditionedByWorldLevel(int tableIndex_, int equipIndex_, int subIndex_, int worldLevel_);


	TArray<FFighterTableStruct> GetAllRecruitedFighters(int tableIndex_);
	TArray<FFighterTableStruct> GetAllFighters(int tableIndex_);
	FFighterTableStruct FindFighterRowById(int tableIndex_, int fighterId_);
	void RemoveFightersDueToPermaDeath(TArray<int>& ids_, int tableIndex_);
	void ClearRecruitedFightersTable(int tableIndex_);
	void IncreaseTheStatsOfThisFigheter(FFighterTableStruct fighter_, int tableIndex_);

	//void ClearRecruitedFightersTable(int tableIndex_);
	void RemoveFighterTableRow(FName rowName_, int tableIndex_);

	//gets a ref to this external file reader
	UExternalFileReader* GetExternalFileReader();

	UDataTable* GetTable(int index_);

	int FindTableIndexByName(FName tableName_);

	void Equip(int fighterTableIndex_, int equipTableIndex, int fighterID, int equipIndex, int equipID,int oldEquipID);
	void RemoveEquipment(int fighterTableIndex_, int equipTableIndex, int fighterID, int equipIndex, int equipID);

	void BuyEquipment(int tableIndex_, int equipId_, int amountToBuy_);
	void SellEquipment(int tableIndex_, int equipId_, int amount_);
	void SellItem(int tableIndex_, int itemID_, int amount_);

	FDayTableStruct GetCurrentDayInfo(int tableIndex_,int dayId_);

	void ClearTable(int tableIndex_);
		
};
