// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameOff24Character.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayAbilitySpec.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/AttributeSetBase.h"
#include "GAS/GOAbilitiesDataAsset.h"
#include "Weapons/GOWeapon.h"

struct FGameplayAbilitySpecHandle;
DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////// AGameOff24Character

AGameOff24Character::AGameOff24Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a SpringArm Component
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SringArm"));
	SpringArmComponent->SetupAttachment(GetCapsuleComponent());
	
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(SpringArmComponent);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

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
	return GetHealth() >= 0.f;
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
	}
	
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			constexpr int32 Priority = 0;
			Subsystem->AddMappingContext(DefaultMappingContext, Priority);
		}
	}

	InitializeAttributes();
	AddCharacterAbilities();
	AddStartupEffects();
	InitAbilitySystem();
}

void AGameOff24Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SpawnDefaultWeapon();
}

void AGameOff24Character::AddWeaponToInventory(AGOWeapon* NewWeapon, bool bEquipWeapon)
{
	CurrentWeapon = NewWeapon;
	NewWeapon->SetOwningCharacter(this);
	NewWeapon->AddAbilities();
}

void AGameOff24Character::SpawnDefaultWeapon()
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

//////////////////////////////////////////////////////////////////////////// Input

void AGameOff24Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGameOff24Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGameOff24Character::Look);

		if (AbilitiesDataAsset)
		{
			const TSet<FGameplayInputAbilityInfo>& InputAbilities = AbilitiesDataAsset->GetInputAbilities();
			for (const auto& It : InputAbilities)
			{
				if (It.IsValid())
				{
					const UInputAction* InputAction = It.InputAction;
					const int32 InputID = It.InputID;
     
					EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &AGameOff24Character::OnAbilityInputPressed, InputID);
					EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &AGameOff24Character::OnAbilityInputReleased, InputID);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AGameOff24Character::Move(const FInputActionValue& Value)
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

void AGameOff24Character::Look(const FInputActionValue& Value)
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

void AGameOff24Character::OnAbilityInputPressed(int32 InputID)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(InputID);
	}
}

void AGameOff24Character::OnAbilityInputReleased(int32 InputID)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputReleased(InputID);
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

const AGOWeapon* AGameOff24Character::GetCurrentWeapon()
{
	return CurrentWeapon;
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

void AGameOff24Character::InitAbilitySystem()
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