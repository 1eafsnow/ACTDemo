// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/ACT_GameplayAbility_Combo.h"
#include "Combo/ACT_ComboComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Character/ACT_CharacterBase.h"
#include "Weapon/ACT_EquipmentComponent.h"
#include "Weapon/ACT_Weapon.h"

UACT_GameplayAbility_Combo::UACT_GameplayAbility_Combo()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UACT_GameplayAbility_Combo::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }

    const AACT_CharacterBase* CharacterBase = ActorInfo ? Cast<AACT_CharacterBase>(ActorInfo->AvatarActor.Get()) : nullptr;

    if (!CharacterBase)
    {
        return false;
    }

    const UACT_EquipmentComponent* Equipment = CharacterBase->GetEquipmentComponent();

    if (!IsValid(Equipment) || !IsValid(Equipment->GetEquippedWeapon()))
    {
        return false;
    }

    return IsValid(Equipment->GetWeaponComboData());
}

void UACT_GameplayAbility_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!IsActive())
    {
        return;
    }

    bEndingAbility = false;
    bOwnsCombo = false;
    ActiveComboComponent = nullptr;
    ActiveMontage = nullptr;
    bTransitionReached = false;
    bCanMoveCancel = false;

    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() || !EntryInputTag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Combo ability is missing an avatar or entry input tag."));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    bOwnsTurnWindow = false;
    TurnWindowCharacter.Reset();

    if (!StartTurnWindowEventTasks())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to start attack turn window event tasks."));
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

    if (!StartWindowEventTasks())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to start combo event tasks."));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    AACT_CharacterBase* CharacterBase = Cast<AACT_CharacterBase>(ActorInfo->AvatarActor.Get());
    UACT_EquipmentComponent* Equipment = CharacterBase ? CharacterBase->GetEquipmentComponent() : nullptr;

    if (!IsValid(Equipment) || !IsValid(Equipment->GetEquippedWeapon()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot start a combo without an equipped weapon."));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UACT_ComboData* SelectedComboData = Equipment->GetWeaponComboData();

    if (!IsValid(SelectedComboData))
    {
        UE_LOG(LogTemp, Warning, TEXT("The equipped weapon has no combo data."));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    FACT_ComboNode EntryAttack;
    if (!ActiveComboComponent->StartCombo(SelectedComboData, EntryInputTag, EntryAttack))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find the combo entry attack."));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    bOwnsCombo = true;

    if (!StartFirstAttack(EntryAttack) && IsActive() && !bEndingAbility)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }
}

bool UACT_GameplayAbility_Combo::StartWindowEventTasks()
{
    const FGameplayTag OpenTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combo.Window.Open"));
    const FGameplayTag TransitionTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combo.Transition"));
    const FGameplayTag CloseTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combo.Window.Close"));
    const FGameplayTag InputTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combo.Input"));
    const FGameplayTag MoveTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Move.Input"));

    UAbilityTask_WaitGameplayEvent* OpenTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, OpenTag, nullptr, false, true);
    UAbilityTask_WaitGameplayEvent* TransitionTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TransitionTag, nullptr, false, true);
    UAbilityTask_WaitGameplayEvent* CloseTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CloseTag, nullptr, false, true);
    UAbilityTask_WaitGameplayEvent* InputTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, InputTag, nullptr, false, true);
    UAbilityTask_WaitGameplayEvent* MoveTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MoveTag, nullptr, false, true);

    if (!OpenTask || !TransitionTask || !CloseTask || !InputTask || !MoveTask)
    {
        return false;
    }

    OpenTask->EventReceived.AddDynamic(this, &UACT_GameplayAbility_Combo::HandleWindowOpen);
    TransitionTask->EventReceived.AddDynamic(this, &UACT_GameplayAbility_Combo::HandleTransition);
    CloseTask->EventReceived.AddDynamic(this, &UACT_GameplayAbility_Combo::HandleWindowClose);
    InputTask->EventReceived.AddDynamic(this, &UACT_GameplayAbility_Combo::HandleComboInput);
    MoveTask->EventReceived.AddDynamic(this, &UACT_GameplayAbility_Combo::HandleMoveInput);

    OpenTask->ReadyForActivation();
    TransitionTask->ReadyForActivation();
    CloseTask->ReadyForActivation();
    InputTask->ReadyForActivation();
    MoveTask->ReadyForActivation();

    return true;
}

bool UACT_GameplayAbility_Combo::StartFirstAttack(const FACT_ComboNode& AttackNode)
{
    if (!ActiveComboComponent || !AttackNode.Montage)
    {
        return false;
    }

    if (!AttackNode.SectionName.IsNone() && AttackNode.Montage->GetSectionIndex(AttackNode.SectionName) == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("Combo section %s does not exist."), *AttackNode.SectionName.ToString());
        return false;
    }

    if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
    {
        return false;
    }

    UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackNode.Montage, 1.0f, AttackNode.SectionName, true);

    if (!MontageTask)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create combo montage task."));
        return false;
    }

    MontageTask->OnCompleted.AddDynamic(this, &UACT_GameplayAbility_Combo::HandleMontageCompleted);
    MontageTask->OnCancelled.AddDynamic(this, &UACT_GameplayAbility_Combo::HandleMontageCancelled);
    MontageTask->OnInterrupted.AddDynamic(this, &UACT_GameplayAbility_Combo::HandleMontageCancelled);

    ActiveMontage = AttackNode.Montage;
    MontageTask->ReadyForActivation();

    if (!IsActive() || bEndingAbility || !ActiveComboComponent)
    {
        return false;
    }

    if (!ActiveComboComponent->ConfirmAttack(true))
    {
        return false;
    }

    ActiveComboComponent->NotifyAttackStarted();
    return true;
}

bool UACT_GameplayAbility_Combo::TransitionToAttack(const FACT_ComboNode& AttackNode)
{
    if (!ActiveComboComponent || !ActiveMontage || AttackNode.Montage != ActiveMontage || AttackNode.SectionName.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("The next attack must use a section in the active montage."));
        return false;
    }

    if (ActiveMontage->GetSectionIndex(AttackNode.SectionName) == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("Combo section %s does not exist."), *AttackNode.SectionName.ToString());
        return false;
    }

    UAnimInstance* AnimInstance = CurrentActorInfo && CurrentActorInfo->SkeletalMeshComponent.IsValid() ? CurrentActorInfo->SkeletalMeshComponent->GetAnimInstance() : nullptr;

    if (!AnimInstance || !AnimInstance->Montage_IsPlaying(ActiveMontage))
    {
        UE_LOG(LogTemp, Warning, TEXT("The combo montage is not playing."));
        return false;
    }

    if (!ActiveComboComponent->ConfirmAttack(true))
    {
        return false;
    }

    bCanMoveCancel = false;
    AnimInstance->Montage_JumpToSection(AttackNode.SectionName, ActiveMontage);
    ActiveComboComponent->NotifyAttackStarted();
    return true;
}

void UACT_GameplayAbility_Combo::HandleWindowOpen(FGameplayEventData Payload)
{
    if (!ActiveComboComponent || !bOwnsCombo || bEndingAbility)
    {
        return;
    }

    bTransitionReached = false;
    ActiveComboComponent->OpenInputWindow();
}

void UACT_GameplayAbility_Combo::HandleTransition(FGameplayEventData Payload)
{
    if (!ActiveComboComponent || !bOwnsCombo || bEndingAbility || !ActiveComboComponent->IsInputWindowOpen())
    {
        return;
    }

    bTransitionReached = true;
    bCanMoveCancel = true;

    TryCommitBufferedInput();

    if (IsActive() && !bEndingAbility && bCanMoveCancel)
    {
        TryCancelForMovement();
    }
}

void UACT_GameplayAbility_Combo::HandleWindowClose(FGameplayEventData Payload)
{
    if (!ActiveComboComponent || !bOwnsCombo || bEndingAbility)
    {
        return;
    }

    bTransitionReached = false;
    ActiveComboComponent->CloseInputWindow();
}

void UACT_GameplayAbility_Combo::HandleComboInput(FGameplayEventData Payload)
{
    if (!bTransitionReached || bEndingAbility)
    {
        return;
    }

    TryCommitBufferedInput();
}

void UACT_GameplayAbility_Combo::TryCommitBufferedInput()
{
    if (!ActiveComboComponent || !bOwnsCombo || bEndingAbility || !bTransitionReached)
    {
        return;
    }

    FACT_ComboNode NextAttack;

    if (!ActiveComboComponent->CommitTransition(NextAttack))
    {
        return;
    }

    bTransitionReached = false;

    if (!TransitionToAttack(NextAttack) && IsActive() && !bEndingAbility)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
    }
}

void UACT_GameplayAbility_Combo::HandleMontageCompleted()
{
    if (!bEndingAbility)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
}

void UACT_GameplayAbility_Combo::HandleMontageCancelled()
{
    if (!bEndingAbility)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
    }
}

void UACT_GameplayAbility_Combo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (bEndingAbility)
    {
        return;
    }

    bEndingAbility = true;

    if (ActiveComboComponent && bOwnsCombo)
    {
        ActiveComboComponent->EndCombo();
    }

    bOwnsCombo = false;
    ActiveMontage = nullptr;
    ActiveComboComponent = nullptr;
    bTransitionReached = false;
    bCanMoveCancel = false;

    if (bOwnsTurnWindow)
    {
        if (AACT_CharacterBase* CharacterBase = TurnWindowCharacter.Get())
        {
            CharacterBase->SetAttackTurnWindow(false);
        }

        bOwnsTurnWindow = false;
        TurnWindowCharacter.Reset();
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UACT_GameplayAbility_Combo::StartTurnWindowEventTasks()
{
    const FGameplayTag OpenTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Attack.TurnWindow.Open"));
    const FGameplayTag CloseTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Attack.TurnWindow.Close"));

    UAbilityTask_WaitGameplayEvent* OpenTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, OpenTag, nullptr, false, true);
    UAbilityTask_WaitGameplayEvent* CloseTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CloseTag, nullptr, false, true);

    if (!OpenTask || !CloseTask)
    {
        return false;
    }

    OpenTask->EventReceived.AddDynamic(this, &UACT_GameplayAbility_Combo::HandleTurnWindowOpen);
    CloseTask->EventReceived.AddDynamic(this, &UACT_GameplayAbility_Combo::HandleTurnWindowClose);

    OpenTask->ReadyForActivation();
    CloseTask->ReadyForActivation();

    return true;
}

void UACT_GameplayAbility_Combo::HandleTurnWindowOpen(FGameplayEventData Payload)
{
    AACT_CharacterBase* CharacterBase = Cast<AACT_CharacterBase>(GetAvatarActorFromActorInfo());

    if (!CharacterBase)
    {
        return;
    }

    TurnWindowCharacter = CharacterBase;
    bOwnsTurnWindow = true;
    CharacterBase->SetAttackTurnWindow(true);
}

void UACT_GameplayAbility_Combo::HandleTurnWindowClose(FGameplayEventData Payload)
{
    if (!bOwnsTurnWindow)
    {
        return;
    }

    if (AACT_CharacterBase* CharacterBase = TurnWindowCharacter.Get())
    {
        CharacterBase->SetAttackTurnWindow(false);
    }

    bOwnsTurnWindow = false;
    TurnWindowCharacter.Reset();
}

void UACT_GameplayAbility_Combo::HandleMoveInput(FGameplayEventData Payload)
{
    TryCancelForMovement();
}

void UACT_GameplayAbility_Combo::TryCancelForMovement()
{
    if (!IsActive() || bEndingAbility || !bCanMoveCancel)
    {
        return;
    }

    AACT_CharacterBase* CharacterBase = Cast<AACT_CharacterBase>(GetAvatarActorFromActorInfo());

    if (!CharacterBase || !CharacterBase->HasMoveIntent())
    {
        return;
    }

    if (ActiveComboComponent && bOwnsCombo)
    {
        ActiveComboComponent->StartResumeWindow();
    }

    bCanMoveCancel = false;

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}