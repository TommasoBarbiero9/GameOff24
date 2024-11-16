// Fill out your copyright notice in the Description page of Project Settings.


#include "GOPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GAS/GOAbilitiesDataAsset.h"
#include "Weapons/GOWeapon.h"

AGOPlayerCharacter::AGOPlayerCharacter()
{
	// Create a SpringArm Component
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SringArm"));
	SpringArmComponent->SetupAttachment(GetCapsuleComponent());
	
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(SpringArmComponent);
	
	GetMesh()->SetupAttachment(SpringArmComponent);
}

void AGOPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SpawnDefaultWeapon();
}

void AGOPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			constexpr int32 Priority = 0;
			Subsystem->AddMappingContext(DefaultMappingContext, Priority);
		}
	}
}

//////////////////////////////////////////////////////////////////////////// Weapon

void AGOPlayerCharacter::SpawnDefaultWeapon()
{
	int32 NumWeaponClasses = DefaultInventoryWeaponClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (!DefaultInventoryWeaponClasses[i])
		{
			// An empty item was added to the Array in blueprint
			continue;
		}

		AGOWeapon* NewWeapon = GetWorld()->SpawnActorDeferred<AGOWeapon>(DefaultInventoryWeaponClasses[i],
			FTransform::Identity, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		NewWeapon->bSpawnWithCollision = false;
		NewWeapon->FinishSpawning(FTransform::Identity);

		bool bEquipFirstWeapon = i == 0;
		AddWeaponToInventory(NewWeapon, bEquipFirstWeapon);
	}
}

void AGOPlayerCharacter::AddWeaponToInventory(AGOWeapon* NewWeapon, bool bEquipWeapon)
{
	CurrentWeapon = NewWeapon;
	NewWeapon->SetOwningCharacter(this);
	NewWeapon->AddAbilities();
}

const AGOWeapon* AGOPlayerCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

//////////////////////////////////////////////////////////////////////////// Input

void AGOPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGOPlayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGOPlayerCharacter::Look);

		if (AbilitiesDataAsset)
		{
			const TSet<FGameplayInputAbilityInfo>& InputAbilities = AbilitiesDataAsset->GetInputAbilities();
			for (const auto& It : InputAbilities)
			{
				if (It.IsValid())
				{
					const UInputAction* InputAction = It.InputAction;
					const int32 InputID = It.InputID;
     
					EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &AGOPlayerCharacter::OnAbilityInputPressed, InputID);
					EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &AGOPlayerCharacter::OnAbilityInputReleased, InputID);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AGOPlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AGOPlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGOPlayerCharacter::OnAbilityInputPressed(int32 InputID)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(InputID);
	}
}

void AGOPlayerCharacter::OnAbilityInputReleased(int32 InputID)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputReleased(InputID);
	}
}

//////////////////////////////////////////////////////////////////////////// GAS

void AGOPlayerCharacter::InitAbilitySystem() const
{
	if (AbilitiesDataAsset)
	{
		const TSet<FGameplayInputAbilityInfo>& InputAbilities = AbilitiesDataAsset->GetInputAbilities();
		constexpr int32 AbilityLevel = 1;
  
		for (const auto& It : InputAbilities)
		{
			if (It.IsValid())
			{
				const FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(It.GameplayAbilityClass, AbilityLevel, It.InputID);
				AbilitySystemComponent->GiveAbility(AbilitySpec);
			}
		}
	}
}