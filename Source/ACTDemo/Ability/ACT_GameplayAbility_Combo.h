// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "Combo/ACT_ComboData.h"
#include "ACT_GameplayAbility_Combo.generated.h"

class UACT_ComboComponent;
class UACT_ComboData;

/**
 * Minimal combo ability that plays the entry attack.
 */
UCLASS()
class ACTDEMO_API UACT_GameplayAbility_Combo : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
    UACT_GameplayAbility_Combo();

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    /** Combo configuration used by this ability. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    TObjectPtr<UACT_ComboData> ComboData;

    /** Input used to select the first combo node. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    FGameplayTag EntryInputTag;

private:
    UFUNCTION()
    void HandleAttackRequested(const FACT_ComboNode& AttackNode);

    UFUNCTION()
    void HandleMontageCompleted();

    UFUNCTION()
    void HandleMontageCancelled();

    UPROPERTY(Transient)
    TObjectPtr<UACT_ComboComponent> ActiveComboComponent;
};
