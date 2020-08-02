// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../ExternalFileReader/FSkillTableStruct.h"
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

	class UGridCharacterAnimInstance* animInstance;

	UPROPERTY(EditAnywhere)
		class UExternalFileReader* fileReader;

	bool bMoving;
	TArray<FVector> movementPath;
	class ATile* originTile;
	UPROPERTY(EditAnywhere, Category = "Grid")
		class UPathComponent* pathComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle") //Needed when we want to call functions of the btl manager from the player's UI (end turn for example)
		class ABattleManager* btlManager;

	EGridCharState currentState;
	AGridCharacter* actionTarget;

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Selected();
	UFUNCTION(BlueprintCallable) //Called from the UI when pressing on End Turn
		virtual void NotSelected();
	void UpdateOriginTile(); //Called at the beginning of every player turn
	void MoveToThisTile(ATile* target_);
	void AttackThisTarget(AGridCharacter* target_, bool skill_); //TODO Add a character to damage to the arguments
	
	//Animation related functions
	UFUNCTION(BlueprintCallable) //Called from within the animation instance
		virtual void ActivateWeaponAttack() {}; //Each fighter will use a different weapon and so each will have a slight different behavior
	UFUNCTION(BlueprintCallable) //Called from within the animation instance
		virtual void ActivateSkillAttack() {}; //Each fighter will use a different weapon and so each will have a slight different behavior


	void SetBattleManager(ABattleManager* btlManager_);
	ATile* GetMyTile(); //Returns the tile the character is standing on
	void SetMoving(bool value_);

	EGridCharState GetCurrentState();
	void GridCharTakeDamage(float damage_, AGridCharacter* attacker_);
	void GridCharReactToSkill(float value_, int statIndex_, int statuEffectIndex_, AGridCharacter* attacker_); //TODO add status effects


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

};
