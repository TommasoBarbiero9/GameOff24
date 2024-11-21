// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameOff24Character.h"
#include "InputMappingContext.h"
#include "GOPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GAMEOFF24_API AGOPlayerCharacter : public AGameOff24Character
{
	GENERATED_BODY()

public:
	AGOPlayerCharacter();
	
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

	/** Returns FirstPersonCameraComponent sub-object **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UGOAbilitiesDataAsset* GetPlayerGameplayAbilitiesDataAsset() const { return AbilitiesDataAsset; }

	// Adds a new weapon to the inventory.
	// Returns false if the weapon already exists in the inventory, true if it's a new weapon.
	UFUNCTION(BlueprintCallable, Category = "GO|Inventory")
	void AddWeaponToInventory(AGOWeapon* NewWeapon, bool bEquipWeapon = false);

	UFUNCTION(BlueprintCallable, Category = "GO|Weapon")
	AGOWeapon* GetCurrentWeapon() const;

protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GO|Weapon")
	TArray<TSubclassOf<AGOWeapon>> DefaultInventoryWeaponClasses;
	
	UPROPERTY()
	AGOWeapon* CurrentWeapon;
	
	// Server spawns default inventory
    void SpawnDefaultWeapon();
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilitySystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGOAbilitiesDataAsset> AbilitiesDataAsset;

	void OnAbilityInputPressed(int32 InputID);
	void OnAbilityInputReleased(int32 InputID);

	void InitAbilitySystem() const;
};
