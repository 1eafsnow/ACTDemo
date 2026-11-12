// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "Combo/ACT_ComboData.h"
#include "ACT_GameplayAbility_Combo.generated.h"

class UACT_ComboComponent;
class UACT_ComboData;
class UAnimMontage;

/**
 * Plays combo attacks from sections of one montage.
 */
UCLASS()
class ACTDEMO_API UACT_GameplayAbility_Combo : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UACT_GameplayAbility_Combo();

    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    FGameplayTag EntryInputTag;

private:
    bool StartFirstAttack(const FACT_ComboNode& AttackNode);
    bool TransitionToAttack(const FACT_ComboNode& AttackNode);
    bool StartWindowEventTasks();

    UFUNCTION()
    void HandleWindowOpen(FGameplayEventData Payload);

    UFUNCTION()
    void HandleTransition(FGameplayEventData Payload);

    UFUNCTION()
    void HandleWindowClose(FGameplayEventData Payload);

    UFUNCTION()
    void HandleMontageCompleted();

    UFUNCTION()
    void HandleMontageCancelled();

    UPROPERTY(Transient)
    TObjectPtr<UACT_ComboComponent> ActiveComboComponent;

    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> ActiveMontage;

    bool bOwnsCombo = false;
    bool bEndingAbility = false;

    void TryCommitBufferedInput();

    UFUNCTION()
    void HandleComboInput(FGameplayEventData Payload);

    bool bTransitionReached = false;

    bool StartTurnWindowEventTasks();

    UFUNCTION()
    void HandleTurnWindowOpen(FGameplayEventData Payload);

    UFUNCTION()
    void HandleTurnWindowClose(FGameplayEventData Payload);

    TWeakObjectPtr<class AACT_CharacterBase> TurnWindowCharacter;
    bool bOwnsTurnWindow = false;

    UFUNCTION()
    void HandleMoveInput(FGameplayEventData Payload);

    void TryCancelForMovement();

    bool bCanMoveCancel = false;
};
