// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/ACT_GameplayAbility_Combo.h"
#include "../Combo/ACT_ComboComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Actor.h"

UACT_GameplayAbility_Combo::UACT_GameplayAbility_Combo()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UACT_GameplayAbility_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() || !ComboData || !EntryInputTag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Combo ability is missing an avatar, combo data, or entry input tag."));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    ActiveComboComponent = ActorInfo->AvatarActor->FindComponentByClass<UACT_ComboComponent>();

    if (!ActiveComboComponent || ActiveComboComponent->IsComboActive())
    {
        UE_LOG(LogTemp, Warning, TEXT("Combo component is unavailable or already active."));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    FACT_ComboNode EntryAttack;
    if (!ActiveComboComponent->StartCombo(ComboData, EntryInputTag, EntryAttack))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to start combo."));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    HandleAttackRequested(EntryAttack);
}

void UACT_GameplayAbility_Combo::HandleAttackRequested(const FACT_ComboNode& AttackNode)
{
    if (!ActiveComboComponent || !AttackNode.Montage)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
        return;
    }

    if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
        return;
    }

    UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackNode.Montage, 1.0f, AttackNode.SectionName, true);

    if (!MontageTask)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create combo montage task."));
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
        return;
    }

    MontageTask->OnCompleted.AddDynamic(this, &UACT_GameplayAbility_Combo::HandleMontageCompleted);
    MontageTask->OnCancelled.AddDynamic(this, &UACT_GameplayAbility_Combo::HandleMontageCancelled);
    MontageTask->OnInterrupted.AddDynamic(this, &UACT_GameplayAbility_Combo::HandleMontageCancelled);

    if (!ActiveComboComponent->ConfirmAttack(true))
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
        return;
    }

    MontageTask->ReadyForActivation();

    if (IsActive() && ActiveComboComponent && ActiveComboComponent->IsComboActive())
    {
        ActiveComboComponent->NotifyAttackStarted();
    }
}

void UACT_GameplayAbility_Combo::HandleMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UACT_GameplayAbility_Combo::HandleMontageCancelled()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UACT_GameplayAbility_Combo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (ActiveComboComponent)
    {
        ActiveComboComponent->EndCombo();
        ActiveComboComponent = nullptr;
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
