// Fill out your copyright notice in the Description page of Project Settings.


#include "GOGameplayAbility_TriggeredInputAction.h"
#include "EnhancedInputComponent.h"
#include "GOAbilitiesDataAsset.h"
#include "GameOff24/GOPlayerCharacter.h"

UGOGameplayAbility_TriggeredInputAction::UGOGameplayAbility_TriggeredInputAction()
{
	bCancelAbilityOnInputReleased = true;
	// InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	// NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
}

void UGOGameplayAbility_TriggeredInputAction::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bool bSuccess = false;

	if (const AGOPlayerCharacter* PlayerCharacter = Cast<AGOPlayerCharacter>(GetAvatarActorFromActorInfo()))
	{
		EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerCharacter->InputComponent);
		if (EnhancedInputComponent)
		{
			if (const UGOAbilitiesDataAsset* AbilitiesDataAsset = PlayerCharacter->GetPlayerGameplayAbilitiesDataAsset())
			{
				const TSet<FGameplayInputAbilityInfo>& InputAbilities = AbilitiesDataAsset->GetInputAbilities();
				for (const auto& It : InputAbilities)
				{
					if (It.IsValid() && It.GameplayAbilityClass == GetClass())
					{
						const FEnhancedInputActionEventBinding& TriggeredEventBinding = EnhancedInputComponent->BindAction(
							It.InputAction, ETriggerEvent::Triggered, this, &UGOGameplayAbility_TriggeredInputAction::OnTriggeredInputAction);
						const uint32 TriggeredEventHandle = TriggeredEventBinding.GetHandle();

						TriggeredEventHandles.AddUnique(TriggeredEventHandle);

						bSuccess = true;
					}
				}
			}
		}
	}

	if (bSuccess)
	{
		// You can skip committing ability here if the ability was committed in the parent class
		CommitAbility(Handle, ActorInfo, ActivationInfo);
	}
	else
	{
		constexpr bool bReplicateCancelAbility = true;
		CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	}
}

void UGOGameplayAbility_TriggeredInputAction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                         const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                                         bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (EnhancedInputComponent)
	{
		for (const uint32 EventHandle : TriggeredEventHandles)
		{
			EnhancedInputComponent->RemoveBindingByHandle(EventHandle);
		}

		EnhancedInputComponent = nullptr;
	}

	TriggeredEventHandles.Reset();
}

void UGOGameplayAbility_TriggeredInputAction::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                            const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	if (bCancelAbilityOnInputReleased)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UGOGameplayAbility_TriggeredInputAction::OnTriggeredInputAction(const FInputActionValue& Value)
{
}
