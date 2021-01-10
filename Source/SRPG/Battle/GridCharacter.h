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
		MOVING,
		ATTACKING,
		SKILLING,
		HEALING,
		FINISHED
	};

	// Sets default values for this character's properties
	AGridCharacter();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(BlueprintReadOnly, Category = "Textures")
		int bpID;

	UPROPERTY(BlueprintReadOnly, Category ="Info Panel")
		FString pName;

protected:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
		class UWidgetComponent* widgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OverheadWidget")
		class UWidgetComponent* overheadWidgetComp;

	UPROPERTY(BlueprintReadOnly, Category = "Champion")
		int championIndex; //-1 neither 0 champion 1 villain 2 perma
	int championBuffCount; //How many time has the champion/villain been buffed debuffed due to the campion status
	UPROPERTY(EditAnywhere, Category = "Champion")
		class UParticleSystemComponent* champParticles;
	UPROPERTY(EditAnywhere, Category = "Champion")
		UParticleSystemComponent* permaChampParticles;
	UPROPERTY(EditAnywhere, Category = "Champion")
		UParticleSystemComponent* villainParticles;

	class UGridCharacterAnimInstance* animInstance;

	UPROPERTY(EditAnywhere, Category="File Reader")
		class UExternalFileReader* fileReader;


	bool bMoving;
	TArray<FVector> movementPath;
	class ATile* originTile;
	UPROPERTY(EditAnywhere, Category = "Grid")
		class UPathComponent* pathComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		class UStatsComponent* statsComp;

	UPROPERTY(BlueprintReadOnly, Category = "Battle") //Needed when we want to call functions of the btl manager from the player's UI (end turn for example)
		class ABattleManager* btlManager;
	class ABattleCrowd* crdManager;

	class ABattleController* btlCtrl;

	EGridCharState currentState;
	TArray<AGridCharacter*> actionTargets;
	class AObstacle* targetObstacle;

	int chosenSkillAnimIndex;
	int fighterIndex;
	int fighterID; //The ID is used to distinguish the fighter inside the recruited table

	UPROPERTY(BlueprintReadOnly, Category = "Skill") //Used by the UI
		FSkillTableStruct chosenSkill;

	//Every turn a character can move and do an action. They can move as much as they want until they do an action.
	//If they do an action before moving, they can still move once.
	UPROPERTY(BlueprintReadOnly, Category = "Structure")
		bool bHasMoved;
	UPROPERTY(BlueprintReadOnly, Category = "Structure")
		bool bHasDoneAnAction;

	UPROPERTY(EditAnywhere, Category = "Weapon")
		TArray<TSubclassOf<class AWeaponBase>> weaponMeshes;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void MoveAccordingToPath();

	UFUNCTION(BlueprintCallable)
		TArray<FItemTableStruct> GetOwnedItems(); //Called by the UI
	UFUNCTION(BlueprintCallable)
		void HighlightItemUsage(FItemTableStruct item_); //Called by UI to highlight the tiles

	FItemTableStruct chosenItem;

	UFUNCTION(BlueprintCallable)
		virtual void ResetCameraFocus();

	virtual void FinishState() {};

	UFUNCTION(BlueprintCallable)
		void RemoveOverheadWidget();

	TArray<AGridCharacter*> TargetedByTheseCharacters; //An array that stores references to characters that target this character. They need to be notified of this character's death lest they use a pointer to it and crash the engine.

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

	void AttackUsingWeapon(AGridCharacter* target_, float delay_);
	void AttackUsingWeapon(AObstacle* obstacle_, float delay_);
	void PlayAnimationAttackUsingWeapon();
	void AttackUsingSkill(TArray<AGridCharacter*> targets_, float delay_, AObstacle* obstacle_);
	void AttackUsingSkill(float delay_); //overloaded for when we can directly push into action targets
	void PlayAnimationAttackUsingSkill();

	void SetBtlAndCrdManagers(ABattleManager* btlManager_,ABattleCrowd* crd_);
	ATile* GetMyTile(); //Returns the tile the character is standing on
	void SetMoving(bool value_);

	EGridCharState GetCurrentState();
	virtual void GridCharTakeDamage(float damage_, AGridCharacter* attacker_, bool crit_, int statusEffect_);
	virtual void GridCharReactToSkill(float value_, int statIndex_, int statuEffectIndex_, AGridCharacter* attacker_, bool crit_);
	virtual void GridCharReatToElemental(float damage_, int statusEffectIndex_);
	void GridCharReactToItem(int statIndex_, int value_);
	void GridCharReactToMiss();
	void UseItemOnOtherChar(AGridCharacter* target_);
	float GetStat(int statIndex_);

	void SetChampionOrVillain(bool value_);
	void UnElect(); //Called by battle crowd when 3 crowd turns pass

	UFUNCTION(BlueprintCallable)
		void Die();
	void SetFighterName(FString name_);

	UExternalFileReader* GetFileReader();
	UStatsComponent* GetStatsComp();
	UPathComponent* GetPathComponent();
	ATile* GetOriginTile();
	class AGridManager* GetGridManager();
	void YouHaveJustKilledAChampion(int championIndex_);
	void YouHaveKilledYouTarget(bool killedAnEnemy_);
	void PassInYourFinalStatsToTheIntermediate(TArray<int>& stats_);

	void YouAreTargetedByMeNow(AGridCharacter* ref_);
	void YouAreNoLongerTargetedByMe(AGridCharacter* ref_);
	virtual void IamDeadStopTargetingMe();//Used to stop characters from targeting a dead character

	void ResetActionTargets(); //Used by blank enemy when switching functions

protected:
	virtual void AddEquipmentStats(int tableIndex_); 

};
