// Fill out your copyright notice in the Description page of Project Settings.


#include "GOGameplayAbility.h"
#include "AbilitySystemComponent.h"


UGOGameplayAbility::UGOGameplayAbility()
{
    // Default to Instance Per Actor
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    bActivateOnInput = true;

    // UGSAbilitySystemGlobals hasn't initialized tags yet to set ActivationBlockedTags
    ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));

    ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("Ability.BlocksInteraction"));
}

bool UGOGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) && GOCheckCost(Handle, *ActorInfo);
}

bool UGOGameplayAbility::GOCheckCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const
{
    return true;
}

void UGOGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
    GOApplyCost(Handle, *ActorInfo, ActivationInfo);
    Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}

bool UGOGameplayAbility::IsInputPressed() const
{
    FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
    return Spec && Spec->InputPressed;
}
