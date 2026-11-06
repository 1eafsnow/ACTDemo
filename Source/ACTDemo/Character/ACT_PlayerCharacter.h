// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ACT_CharacterBase.h"
#include "ACT_PlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

/**
 * Player character with a third-person camera.
 * Input is handled by the player controller.
 */
UCLASS()
class ACTDEMO_API AACT_PlayerCharacter : public AACT_CharacterBase
{
	GENERATED_BODY()

public:
    AACT_PlayerCharacter();

protected:
    // Controls camera distance and follows the controller's rotation.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    // Third-person player camera.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> FollowCamera;
	
};
