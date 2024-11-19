// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Logging/LogMacros.h"
#include "GameplayEffectTypes.h"
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGSOnGameplayAttributeValueChangedDelegate, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

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

	UPROPERTY(BlueprintAssignable, Category = "GO|GOCharacter")
	FCharacterDiedDelegate OnCharacterDied;

	virtual void RemoveCharacterAbilities();
	
	virtual void Die();

	void FinishDying();

	UFUNCTION(BlueprintCallable, Category = "GO|GOCharacter|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GO|GOCharacter|Attributes")
	float GetMaxHealth() const;

protected:
	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	UPROPERTY(EditAnywhere)
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	const class UAttributeSetBase* AttributeSetBase;
	
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

	// Attribute changed delegate handles
	FDelegateHandle HealthChangedDelegateHandle;
	
	virtual void BeginPlay() override;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	
	virtual void AddCharacterAbilities();

	// Initialize the Character's attributes.
	virtual void InitializeAttributes();

	virtual void AddStartupEffects();
};

