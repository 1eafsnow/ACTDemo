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

void AACT_Weapon::InitializeWeapon(UACT_WeaponData* InDefinition)
{
    WeaponDefinition = InDefinition;
    WeaponMesh->SetStaticMesh(InDefinition ? InDefinition->WeaponMesh.Get() : nullptr);
}

UACT_WeaponData* AACT_Weapon::GetWeaponDefinition() const
{
    return WeaponDefinition;
}

