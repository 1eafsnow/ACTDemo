// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ACT_WeaponData.generated.h"

class AACT_Weapon;
class UACT_ComboData;
class UAnimInstance;
class UStaticMesh;

/**
 * Configuration shared by weapons of the same definition.
 */
UCLASS(BlueprintType)
class ACTDEMO_API UACT_WeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
    // Actor class spawned when this weapon is equipped.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AACT_Weapon> WeaponActorClass;

    // Weapon mesh attached to the character.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<UStaticMesh> WeaponMesh;

    // Socket on the character mesh used to attach the weapon.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName AttachSocket = NAME_None;

    // Local transform applied after attaching the weapon.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FTransform AttachOffset = FTransform::Identity;

    // Base attack power used by the future damage system.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Attributes", meta = (ClampMin = "0.0"))
    float AttackPower = 10.0f;

    // Multiplier applied to attack-node stamina costs by the future stamina system.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Attributes", meta = (ClampMin = "0.0"))
    float StaminaCostMultiplier = 1.0f;

    // Combo configuration used while this weapon is equipped.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat")
    TObjectPtr<UACT_ComboData> ComboData;

    // Optional animation blueprint used while this weapon is equipped.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
    TSubclassOf<UAnimInstance> AnimationClass;
	
};
