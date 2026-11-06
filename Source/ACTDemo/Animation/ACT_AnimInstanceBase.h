// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ACT_AnimInstanceBase.generated.h"

/**
 * Common animation data for player and enemy characters.
 */
UCLASS()
class ACTDEMO_API UACT_AnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()
	
public:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    // Horizontal movement speed in centimeters per second.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Movement")
    float GroundSpeed = 0.0f;

    // Whether the character is falling or jumping.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Movement")
    bool bIsInAir = false;

    // Whether the movement component currently has acceleration.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Movement")
    bool bHasAcceleration = false;
};
