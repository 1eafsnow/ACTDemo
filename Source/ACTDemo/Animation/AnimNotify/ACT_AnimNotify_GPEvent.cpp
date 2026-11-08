// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/ACT_AnimNotify_GPEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

UACT_AnimNotify_GPEvent::UACT_AnimNotify_GPEvent()
{
    bShouldFireInEditor = false;
}

void UACT_AnimNotify_GPEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    AActor* OwnerActor = MeshComp ? MeshComp->GetOwner() : nullptr;

    if (!OwnerActor || !EventTag.IsValid())
    {
        return;
    }

    FGameplayEventData EventData;
    EventData.EventTag = EventTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, EventData);
}
