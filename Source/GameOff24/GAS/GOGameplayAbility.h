// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameOff24/GameOff24.h"
#include "GOGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GAMEOFF24_API UGOGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGOGameplayAbility();

	// Abilities with this set will automatically activate when the input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGOAbilityInputID AbilityInputID = EGOAbilityInputID::None;

	// If true, this ability will activate when its bound input is pressed. Disable if you want to bind an ability to an
	// input but not have it activate when pressed.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool bActivateOnInput;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;

	// Allows C++ and Blueprint abilities to override how cost is checked in case they don't use a GE like weapon ammo
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	bool GOCheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const;
	virtual bool GOCheckCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const;

	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	// Allows C++ and Blueprint abilities to override how cost is applied in case they don't use a GE like weapon ammo
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	void GOApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const;
	virtual void GOApplyCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const {};

	// Is the player's input currently pressed? Only works if the ability is bound to input.
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual bool IsInputPressed() const;
};
