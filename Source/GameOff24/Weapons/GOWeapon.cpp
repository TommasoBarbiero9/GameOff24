// Fill out your copyright notice in the Description page of Project Settings.


#include "GOWeapon.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GameplayAbilitySpec.h"
#include "Components/CapsuleComponent.h"
#include "GameOff24/GameOff24Character.h"
#include "GameOff24/GAS/GOGameplayAbility.h"

// Sets default values
AGOWeapon::AGOWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bSpawnWithCollision = true;

	WeaponPrimaryInstantAbilityTag = FGameplayTag::RequestGameplayTag("Ability.Weapon.Primary.Instant");
	WeaponIsFiringTag = FGameplayTag::RequestGameplayTag("Weapon.IsFiring");

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(FName("CollisionComponent"));
	CollisionComp->InitCapsuleSize(40.0f, 50.0f);
	CollisionComp->SetCollisionObjectType(COLLISION_PICKUP);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Manually enable when in pickup mode
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CollisionComp;

	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(FName("WeaponMesh1P"));
	WeaponMesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh1P->CastShadow = false;
	WeaponMesh1P->SetVisibility(false, true);
	WeaponMesh1P->SetupAttachment(CollisionComp);
	WeaponMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
}

UAbilitySystemComponent* AGOWeapon::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGOWeapon::SetOwningCharacter(AGameOff24Character* InOwningCharacter)
{
	OwningCharacter = InOwningCharacter;
	if (OwningCharacter)
	{
		// Called when added to inventory
		AbilitySystemComponent = Cast<UAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());
		SetOwner(InOwningCharacter);
		AttachToComponent(OwningCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		AbilitySystemComponent = nullptr;
		SetOwner(nullptr);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void AGOWeapon::AddAbilities()
{
	UAbilitySystemComponent* ASC = Cast<UAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());

	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (TSubclassOf<UGOGameplayAbility>& Ability : Abilities)
	{
		AbilitySpecHandles.Add(ASC->GiveAbility(
			FGameplayAbilitySpec(Ability, 1, static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), this)));
		UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(OwningCharacter->InputComponent);
	}
}

void AGOWeapon::RemoveAbilities()
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetAbilitySystemComponent())
	{
		return;
	}

	UAbilitySystemComponent* ASC = Cast<UAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());

	if (!ASC)
	{
		return;
	}

	// Remove abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (FGameplayAbilitySpecHandle& SpecHandle : AbilitySpecHandles)
	{
		ASC->ClearAbility(SpecHandle);
	}	
}

// void AGOWeapon::Equip()
// {
// 	
// }
//
// void AGOWeapon::UnEquip()
// {
// 	
// }

// Called when the game starts or when spawned
void AGOWeapon::BeginPlay()
{
	
}

void AGOWeapon::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

