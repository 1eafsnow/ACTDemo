// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/ACT_AnimNotifyState_GPEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

UACT_AnimNotifyState_GPEvent::UACT_AnimNotifyState_GPEvent()
{
    bShouldFireInEditor = false;
}

void UACT_AnimNotifyState_GPEvent::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    AActor* OwnerActor = MeshComp ? MeshComp->GetOwner() : nullptr;

    if (!OwnerActor || !BeginEventTag.IsValid())
    {
        return;
    }

    FGameplayEventData EventData;
    EventData.EventTag = BeginEventTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, BeginEventTag, EventData);
}

void UACT_AnimNotifyState_GPEvent::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    AActor* OwnerActor = MeshComp ? MeshComp->GetOwner() : nullptr;

    if (!OwnerActor || !EndEventTag.IsValid())
    {
        return;
    }

    FGameplayEventData EventData;
    EventData.EventTag = EndEventTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EndEventTag, EventData);
}
