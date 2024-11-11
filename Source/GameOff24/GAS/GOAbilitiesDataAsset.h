// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GOGameplayAbility.h"
#include "GOAbilitiesDataAsset.generated.h"

class UInputAction;

USTRUCT()
struct FGameplayInputAbilityInfo
{
	GENERATED_USTRUCT_BODY()
 
	UPROPERTY(EditAnywhere, Category = "GameplayInputAbilityInfo")
	TSubclassOf<UGameplayAbility> GameplayAbilityClass;

	UPROPERTY(EditAnywhere, Category = "GameplayInputAbilityInfo")
	TObjectPtr<UInputAction> InputAction;
 
	/** It will be generated automatically. */
	UPROPERTY(VisibleAnywhere, Category = "GameplayInputAbilityInfo")
	int32 InputID;

	bool IsValid() const
	{
		return GameplayAbilityClass && InputAction;
	}
 
	bool operator==(const FGameplayInputAbilityInfo& Other) const
	{
		return GameplayAbilityClass == Other.GameplayAbilityClass && InputAction == Other.InputAction;
	}

	bool operator!=(const FGameplayInputAbilityInfo& Other) const
	{
		return !operator==(Other);
	}

	friend uint32 GetTypeHash(const FGameplayInputAbilityInfo& Item)
	{
		return HashCombine(GetTypeHash(Item.GameplayAbilityClass), GetTypeHash(Item.InputAction));
	}

	FGameplayInputAbilityInfo()
  : InputID(INDEX_NONE)
	{
	}
};

/**
 * 
 */
UCLASS()
class GAMEOFF24_API UGOAbilitiesDataAsset : public UDataAsset
{
	GENERATED_BODY()

	UGOAbilitiesDataAsset();

public:
	const TSet<FGameplayInputAbilityInfo>& GetInputAbilities() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TSet<FGameplayInputAbilityInfo> InputAbilities;
};
