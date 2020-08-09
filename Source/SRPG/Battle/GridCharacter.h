// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../ExternalFileReader/FSkillTableStruct.h"
#include "../ExternalFileReader/ItemTableStruct.h"
#include "GridCharacter.generated.h"

UCLASS()
class SRPG_API AGridCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	enum class EGridCharState : unsigned short
	{
		IDLE,
		ATTACKING,
		SKILLING,
		HEALING
	};

	// Sets default values for this character's properties
	AGridCharacter();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

protected:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
		class UWidgetComponent* widgetComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Champion")
		UWidgetComponent* widgetOnTopOfHead;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Champion")
		bool bChampion;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Champion")
		bool bVillain;
	UPROPERTY(EditAnywhere, Category = "Champion")
		class UParticleSystemComponent* champParticles;
	UPROPERTY(EditAnywhere, Category = "Champion")
		UParticleSystemComponent* villainParticles;

	class UGridCharacterAnimInstance* animInstance;

	UPROPERTY(EditAnywhere)
		class UExternalFileReader* fileReader;

	bool bMoving;
	TArray<FVector> movementPath;
	class ATile* originTile;
	UPROPERTY(EditAnywhere, Category = "Grid")
		class UPathComponent* pathComp;

	UPROPERTY(BlueprintReadOnly, Category = "Battle") //Needed when we want to call functions of the btl manager from the player's UI (end turn for example)
		class ABattleManager* btlManager;
	class ABattleCrowd* crdManager;

	EGridCharState currentState;
	TArray<AGridCharacter*> actionTargets;

	TArray<FSkillTableStruct> skills;
	int chosenSkill;
	int chosenSkillAnimIndex;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void MoveAccordingToPath();

	UFUNCTION(BlueprintCallable)
		TArray<FSkillTableStruct> GetCharacterSkills();
	UFUNCTION(BlueprintCallable)
		void UseSkill(int index_);


	UFUNCTION(BlueprintCallable)
		TArray<FItemTableStruct> GetOwnedItems(); //Called by the UI
	UFUNCTION(BlueprintCallable)
		void HighlightItemUsage(FName itemName_); //Called by UI to highlight the tiles

	FName chosenItem;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Selected();
	UFUNCTION(BlueprintCallable) //Called from the UI when pressing on End Turn
		virtual void NotSelected();
	void UpdateOriginTile(); //Called at the beginning of every player turn
	void MoveToThisTile(ATile* target_);
	
	
	//Animation related functions
	UFUNCTION(BlueprintCallable) //Called from within the animation instance
		virtual void ActivateWeaponAttack() {}; //Each fighter will use a different weapon and so each will have a slight different behavior
	UFUNCTION(BlueprintCallable) //Called from within the animation instance
		virtual void ActivateSkillAttack() {}; //Each fighter will use a different weapon and so each will have a slight different behavior

	void AttackUsingWeapon(AGridCharacter* target_);
	void AttackUsingSkill(TArray<AGridCharacter*> targets_);

	void SetBtlAndCrdManagers(ABattleManager* btlManager_,ABattleCrowd* crd_);
	ATile* GetMyTile(); //Returns the tile the character is standing on
	void SetMoving(bool value_);

	EGridCharState GetCurrentState();
	void GridCharTakeDamage(float damage_, AGridCharacter* attacker_);
	void GridCharReactToSkill(float value_, int statIndex_, int statuEffectIndex_, AGridCharacter* attacker_); //TODO add status effects
	void GridCharReactToItem(int statIndex_, int value_);
	void UseItemOnOtherChar(AGridCharacter* target_);
	float GetStat(int statIndex_);

	void SetChampionOrVillain(bool value_);
	void UnElect(); //Called by battle crowd when 3 crowd turns pass
	


	//PLACEHOLDERS PLACEHOLDERS PLACEHOLDERS PLACEHOLDERS
protected:

	//PLACEHOLDERS. WILL GET THIS STUFF FROM THE TABLES
	UPROPERTY(EditAnywhere)
		int attackRowSpeed;
	UPROPERTY(EditAnywhere)
		int attackDepth;
	UPROPERTY(EditAnywhere)
		int weaponIndex;
	UPROPERTY(EditAnywhere)
		int currentLevel;
	UPROPERTY(EditAnywhere)
		int crd;
	UPROPERTY(EditAnywhere)
		int currentCrdPoints;

};
