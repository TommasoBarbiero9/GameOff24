// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameOff24PlayerController.generated.h"

class UInputMappingContext;

/**
 *
 */
UCLASS()
class GAMEOFF24_API AGameOff24PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void CreateHUD();
	
	class UGOHUDWidget* GetGOHUD() const;

	// Weapon
	// UFUNCTION(BlueprintCallable, Category = "GO|UI")
	// void SetEquippedWeaponPrimaryIconFromSprite(UPaperSprite* InSprite);

	// Reticle
	UFUNCTION(BlueprintCallable, Category = "GO|UI")
	void SetHUDReticle(TSubclassOf<class UGOHUDReticle> ReticleClass);
	
protected:
	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GO|UI")
	TSubclassOf<class UGOHUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "GO|UI")
	class UGOHUDWidget* UIHUDWidget;
};
