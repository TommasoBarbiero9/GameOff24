// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AttributeSetBase.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName,PropertyName)\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName,PropertyName)\
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class GAMEOFF24_API UAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Strength);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxStrength;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxStrength);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Speed);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxSpeed;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Armor);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxArmor;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxArmor);
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Damage);

protected:
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
};
