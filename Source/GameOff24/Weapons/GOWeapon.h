// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "GOWeapon.generated.h"

struct FInputActionValue;
struct FGameplayAbilitySpecHandle;
class AGameOff24Character;
class UPaperSprite;
class UGOGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponAmmoChangedDelegate, int32, OldValue, int32, NewValue);

UCLASS()
class GAMEOFF24_API AGOWeapon : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGOWeapon();

	UPROPERTY(BlueprintAssignable, Category = "GO|GOWeapon")
	FWeaponAmmoChangedDelegate OnPrimaryClipAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "GO|GOWeapon")
	FWeaponAmmoChangedDelegate OnMaxPrimaryClipAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "GO|GOWeapon")
	FWeaponAmmoChangedDelegate OnSecondaryClipAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "GO|GOWeapon")
	FWeaponAmmoChangedDelegate OnMaxSecondaryClipAmmoChanged;

	// UI HUD Primary Icon when equipped.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GO|GOWeapon")
	UPaperSprite* WeaponIcon;
	
	// Whether or not to spawn this weapon with collision enabled (pickup mode).
	// Set to false when spawning directly into a player's inventory or true when spawning into the world in pickup mode.
	UPROPERTY(BlueprintReadWrite)
	bool bSpawnWithCollision;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "GO|GOWeapon")
	TSubclassOf<UGOHUDReticle> GetHUDReticleClass();

	void SetOwningCharacter(AGameOff24Character* InOwningCharacter);

	// // Called when the player equips this weapon
	// virtual void Equip();
	//
	// // Called when the player un-equips this weapon
	// virtual void UnEquip();

	virtual void AddAbilities();

	virtual void RemoveAbilities();

protected:
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "GO|GOWeapon")
	AGameOff24Character* OwningCharacter;

	UPROPERTY(EditAnywhere, Category = "GO|GOWeapon")
	TArray<TSubclassOf<UGOGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadOnly, Category = "GO|GOWeapon")
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
	
	// Cache tags
	FGameplayTag WeaponPrimaryInstantAbilityTag;
	FGameplayTag WeaponIsFiringTag;

	// Collision capsule for when weapon is in pickup mode
	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = "GO|GOWeapon")
	USkeletalMeshComponent* WeaponMesh1P;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GO|UI")
	TSubclassOf<class UGOHUDReticle> HUDReticleClass;

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
};
