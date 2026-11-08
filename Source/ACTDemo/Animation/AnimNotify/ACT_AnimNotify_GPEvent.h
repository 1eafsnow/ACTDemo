// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "ACT_AnimNotify_GPEvent.generated.h"

/**
 * Sends a gameplay event to the owning actor.
 */
UCLASS(meta = (DisplayName = "ACT Gameplay Event"))
class ACTDEMO_API UACT_AnimNotify_GPEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:
    UACT_AnimNotify_GPEvent();

    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
    // Gameplay event sent when the notify is triggered.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Event")
    FGameplayTag EventTag;
};
