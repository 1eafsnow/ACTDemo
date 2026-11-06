// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ACT_AnimInstanceBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UACT_AnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    const ACharacter* OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());

    if (!OwnerCharacter)
    {
        GroundSpeed = 0.0f;
        bIsInAir = false;
        bHasAcceleration = false;
        return;
    }

    const UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();

    if (!Movement)
    {
        GroundSpeed = 0.0f;
        bIsInAir = false;
        bHasAcceleration = false;
        return;
    }

    GroundSpeed = OwnerCharacter->GetVelocity().Size2D();
    bIsInAir = Movement->IsFalling();
    bHasAcceleration = !Movement->GetCurrentAcceleration().IsNearlyZero();
}
