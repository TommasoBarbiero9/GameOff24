// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameOff24PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "GOPlayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "UI/GOHUDReticle.h"
#include "UI/GOHUDWidget.h"
#include "Weapons/GOWeapon.h"

void AGameOff24PlayerController::CreateHUD()
{
	// Only create once
	if (UIHUDWidget) return;

	if (!UIHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}

	AGOPlayerCharacter* PlayerChar = Cast<AGOPlayerCharacter>(GetCharacter());
	if (!PlayerChar) return;

	UIHUDWidget = CreateWidget<UGOHUDWidget>(this, UIHUDWidgetClass);
	UIHUDWidget->AddToViewport();

	UIHUDWidget->SetCurrentHealth(PlayerChar->GetHealth());
	UIHUDWidget->SetMaxHealth(PlayerChar->GetMaxHealth());
	UIHUDWidget->SetHealthPercentage(PlayerChar->GetHealth() / PlayerChar->GetMaxHealth());

	AGOWeapon* CurrentWeapon = PlayerChar->GetCurrentWeapon();
	if (CurrentWeapon)
	{
		UIHUDWidget->SetEquippedWeaponSprite(CurrentWeapon->WeaponIcon);
		UIHUDWidget->SetReticle(CurrentWeapon->GetHUDReticleClass());
	}
}

UGOHUDWidget* AGameOff24PlayerController::GetGOHUD() const
{
	return UIHUDWidget;
}

void AGameOff24PlayerController::SetHUDReticle(TSubclassOf<UGOHUDReticle> ReticleClass)
{
	// !GetWorld()->bIsTearingDown Stops an error when quitting PIE while targeting when the EndAbility resets the HUD reticle
	if (UIHUDWidget && GetWorld() && !GetWorld()->bIsTearingDown)
	{
		UIHUDWidget->SetReticle(ReticleClass);
	}
}

void AGameOff24PlayerController::BeginPlay()
{
	Super::BeginPlay();

	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}
