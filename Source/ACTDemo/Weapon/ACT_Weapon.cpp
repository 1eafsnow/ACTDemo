// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ACT_Weapon.h"
#include "ACT_WeaponData.h"
#include "Components/StaticMeshComponent.h"

AACT_Weapon::AACT_Weapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    SetRootComponent(WeaponMesh);

    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AACT_Weapon::InitializeWeapon(UACT_WeaponData* InData)
{
    WeaponData = InData;
    WeaponMesh->SetStaticMesh(InData ? InData->WeaponMesh.Get() : nullptr);
}

UACT_WeaponData* AACT_Weapon::GetWeaponData() const
{
    return WeaponData;
}

