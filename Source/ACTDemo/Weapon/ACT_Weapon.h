// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACT_Weapon.generated.h"

class UACT_WeaponData;
class UStaticMeshComponent;

/**
 * Basic weapon actor attached to a character.
 */
UCLASS(Blueprintable)
class ACTDEMO_API AACT_Weapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AACT_Weapon();

	/** Applies the weapon definition to this actor. */
	void InitializeWeapon(UACT_WeaponData* InDefinition);

	UFUNCTION(BlueprintPure, Category = "Weapon")
	UACT_WeaponData* GetWeaponDefinition() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UACT_WeaponData> WeaponDefinition;

};
