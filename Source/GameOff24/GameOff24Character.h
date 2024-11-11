// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Logging/LogMacros.h"
#include "GameOff24Character.generated.h"

class UGOAbilitiesDataAsset;
class AGOWeapon;
class USpringArmComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AGameOff24Character*, Character);

UCLASS(config=Game)
class AGameOff24Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	AGameOff24Character();

	// Implement IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "GO|GOCharacter")
	virtual bool IsAlive() const;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** Camera Spring Arm */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(BlueprintAssignable, Category = "GO|GOCharacter")
	FCharacterDiedDelegate OnCharacterDied;

	/** Returns Mesh1P sub-object **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	
	/** Returns FirstPersonCameraComponent sub-object **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UGOAbilitiesDataAsset* GetPlayerGameplayAbilitiesDataAsset() const { return AbilitiesDataAsset; }

	// Adds a new weapon to the inventory.
	// Returns false if the weapon already exists in the inventory, true if it's a new weapon.
	UFUNCTION(BlueprintCallable, Category = "GO|Inventory")
	void AddWeaponToInventory(AGOWeapon* NewWeapon, bool bEquipWeapon = false);

	virtual void RemoveCharacterAbilities();
	
	virtual void Die();

	void FinishDying();

	UFUNCTION(BlueprintCallable, Category = "GO|GOCharacter|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GO|GOCharacter|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GO|Weapon")
	const AGOWeapon* GetCurrentWeapon();

protected:
	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	UPROPERTY(EditAnywhere)
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	const class UAttributeSetBase* AttributeSetBase;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GO|Weapon")
	TArray<TSubclassOf<AGOWeapon>> DefaultInventoryWeaponClasses;
	
	// Default abilities for this Character. These will be removed on Character death and re-given if Character respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GO|Abilities")
	TArray<TSubclassOf<class UGOGameplayAbility>> CharacterAbilities;

	// Default attributes for a character for initializing on spawn/respawn.
	// This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GO|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	// These effects are only applied one time on startup
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GO|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	UPROPERTY()
	AGOWeapon* CurrentWeapon;
	
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	virtual void AddCharacterAbilities();

	// Initialize the Character's attributes.
	virtual void InitializeAttributes();

	virtual void AddStartupEffects();
	void InitAbilitySystem();

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	// Server spawns default inventory
	void SpawnDefaultWeapon();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilitySystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGOAbilitiesDataAsset> AbilitiesDataAsset;

	void OnAbilityInputPressed(int32 InputID);
	void OnAbilityInputReleased(int32 InputID);

};

