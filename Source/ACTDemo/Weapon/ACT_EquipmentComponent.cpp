// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/ACT_EquipmentComponent.h"
#include "ACT_Weapon.h"
#include "ACT_WeaponData.h"
#include "Animation/AnimInstance.h"
#include "Combo/ACT_ComboComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

UACT_EquipmentComponent::UACT_EquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UACT_EquipmentComponent::BeginPlay()
{
    Super::BeginPlay();

    if (DefaultWeapon && !EquipWeapon(DefaultWeapon))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to equip the default weapon for %s."), *GetNameSafe(GetOwner()));
    }
}

void UACT_EquipmentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    DestroyEquippedWeapon();
    Super::EndPlay(EndPlayReason);
}

bool UACT_EquipmentComponent::EquipWeapon(UACT_WeaponData* Data)
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

    if (!OwnerCharacter || !OwnerCharacter->GetMesh() || !GetWorld() || !IsValid(Data))
    {
        return false;
    }

    if (IsComboActive())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot change weapons during an active combo."));
        return false;
    }

    if (!Data->AttachSocket.IsNone() && !OwnerCharacter->GetMesh()->DoesSocketExist(Data->AttachSocket))
    {
        UE_LOG(LogTemp, Warning, TEXT("Weapon socket %s was not found."), *Data->AttachSocket.ToString());
        return false;
    }

    TSubclassOf<AACT_Weapon> WeaponClass = Data->WeaponActorClass;

    if (!WeaponClass)
    {
        WeaponClass = AACT_Weapon::StaticClass();
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = OwnerCharacter;
    SpawnParameters.Instigator = OwnerCharacter;

    AACT_Weapon* NewWeapon = GetWorld()->SpawnActor<AACT_Weapon>(WeaponClass, OwnerCharacter->GetActorTransform(), SpawnParameters);

    if (!NewWeapon)
    {
        return false;
    }

    NewWeapon->InitializeWeapon(Data);

    const bool bAttached = NewWeapon->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Data->AttachSocket);

    if (!bAttached)
    {
        NewWeapon->Destroy();
        return false;
    }

    NewWeapon->SetActorRelativeTransform(Data->AttachOffset);

    if (!EquippedWeapon)
    {
        PreviousAnimationClass = OwnerCharacter->GetMesh()->GetAnimClass();
    }

    DestroyEquippedWeapon();

    EquippedWeapon = NewWeapon;
    EquippedData = Data;

    if (Data->AnimationClass)
    {
        OwnerCharacter->GetMesh()->SetAnimInstanceClass(Data->AnimationClass);
    }
    else if (PreviousAnimationClass)
    {
        OwnerCharacter->GetMesh()->SetAnimInstanceClass(PreviousAnimationClass);
    }

    return true;
}

bool UACT_EquipmentComponent::UnequipWeapon()
{
    if (IsComboActive())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot unequip a weapon during an active combo."));
        return false;
    }

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

    DestroyEquippedWeapon();

    if (OwnerCharacter && OwnerCharacter->GetMesh() && PreviousAnimationClass)
    {
        OwnerCharacter->GetMesh()->SetAnimInstanceClass(PreviousAnimationClass);
    }

    PreviousAnimationClass = nullptr;
    return true;
}

AACT_Weapon* UACT_EquipmentComponent::GetEquippedWeapon() const
{
    return EquippedWeapon;
}

UACT_WeaponData* UACT_EquipmentComponent::GetWeaponData() const
{
    return EquippedData;
}

UACT_ComboData* UACT_EquipmentComponent::GetWeaponComboData() const
{
    return EquippedData ? EquippedData->ComboData.Get() : nullptr;
}

bool UACT_EquipmentComponent::IsComboActive() const
{
    const UACT_ComboComponent* ComboComponent = GetOwner() ? GetOwner()->FindComponentByClass<UACT_ComboComponent>() : nullptr;
    return ComboComponent && ComboComponent->IsComboActive();
}

void UACT_EquipmentComponent::DestroyEquippedWeapon()
{
    if (IsValid(EquippedWeapon))
    {
        EquippedWeapon->Destroy();
    }

    EquippedWeapon = nullptr;
    EquippedData = nullptr;
}

