// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameOff24Character.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayAbilitySpec.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AttributeSetBase.h"
#include "GAS/GOGameplayAbility.h"


struct FGameplayAbilitySpecHandle;
DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////// AGameOff24Character

AGameOff24Character::AGameOff24Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	DeadTag = FGameplayTag::RequestGameplayTag("State.Dead");
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag("Effect.RemoveOnDeath");

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
}

UAbilitySystemComponent* AGameOff24Character::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool AGameOff24Character::IsAlive() const
{
	return GetHealth() > 0.f;
}

void AGameOff24Character::Die()
{
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	OnCharacterDied.Broadcast(this);

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	// //TODO replace with a locally executed GameplayCue
	// if (DeathSound)
	// {
	// 	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	// }
	//
	// if (DeathMontage)
	// {
	// 	PlayAnimMontage(DeathMontage);
	// }
	// else
	// {
		FinishDying();
	// }
}

void AGameOff24Character::FinishDying()
{
	Destroy();
}

void AGameOff24Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	if (IsValid(AbilitySystemComponent))
	{
		AttributeSetBase = AbilitySystemComponent->GetSet<UAttributeSetBase>();
		
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AGameOff24Character::HealthChanged);
	}

	InitializeAttributes();
	AddCharacterAbilities();
	AddStartupEffects();
}

void AGameOff24Character::HealthChanged(const FOnAttributeChangeData& Data)
{
	// Check for and handle death
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		Die();
	}
}

//////////////////////////////////////////////////////////////////////////// GAS

void AGameOff24Character::RemoveCharacterAbilities()
{
	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}
}

float AGameOff24Character::GetHealth() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}

float AGameOff24Character::GetMaxHealth() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.0f;
}

void AGameOff24Character::AddCharacterAbilities()
{
	for (TSubclassOf<UGOGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}
}

void AGameOff24Character::InitializeAttributes()
{
	if (!IsValid(AbilitySystemComponent)) return;

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
	}
}

void AGameOff24Character::AddStartupEffects()
{
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
		}
	}
}