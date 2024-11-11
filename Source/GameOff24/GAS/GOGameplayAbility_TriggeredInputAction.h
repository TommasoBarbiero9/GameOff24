// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOGameplayAbility.h"
#include "GOGameplayAbility_TriggeredInputAction.generated.h"

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class GAMEOFF24_API UGOGameplayAbility_TriggeredInputAction : public UGOGameplayAbility
{
	GENERATED_BODY()

	UGOGameplayAbility_TriggeredInputAction();
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	bool bCancelAbilityOnInputReleased;

protected:
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent;

	TArray<uint32> TriggeredEventHandles;

	//~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
public:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~ End UGameplayAbility Interface

protected:
	//~ Begin UGameplayAbility_BaseTriggeredInputActionAbility Interface
	virtual void OnTriggeredInputAction(const FInputActionValue& Value);
	//~ End UGameplayAbility_BaseTriggeredInputActionAbility Interface
};
