// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "ACT_AnimNotifyState_GPEvent.generated.h"

/**
 * Sends gameplay events when the notify state begins and ends.
 */
UCLASS(meta = (DisplayName = "ACT Gameplay Event Window"))
class ACTDEMO_API UACT_AnimNotifyState_GPEvent : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
    UACT_AnimNotifyState_GPEvent();

    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
    // Event sent when the window begins.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Event")
    FGameplayTag BeginEventTag;

    // Event sent when the window ends.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Event")
    FGameplayTag EndEventTag;
};
