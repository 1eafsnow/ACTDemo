// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACT_EquipmentComponent.generated.h"

class AACT_Weapon;
class UACT_WeaponData;
class UACT_ComboData;

/**
 * Manages the character's equipped weapon.
 */
UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class ACTDEMO_API UACT_EquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACT_EquipmentComponent();

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipWeapon(UACT_WeaponData* Data);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool UnequipWeapon();

	UFUNCTION(BlueprintPure, Category = "Equipment")
	AACT_Weapon* GetEquippedWeapon() const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	UACT_WeaponData* GetWeaponData() const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	UACT_ComboData* GetWeaponComboData() const;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Weapon equipped automatically when play begins.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<UACT_WeaponData> DefaultWeapon;

public:	
	bool IsComboActive() const;
	void DestroyEquippedWeapon();

	UPROPERTY(Transient)
	TObjectPtr<AACT_Weapon> EquippedWeapon;

	UPROPERTY(Transient)
	TObjectPtr<UACT_WeaponData> EquippedData;

	UPROPERTY(Transient)
	TSubclassOf<UAnimInstance> PreviousAnimationClass;

		
};
