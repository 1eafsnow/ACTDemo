// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/ACT_PlayerController.h"
#include "Character/ACT_CharacterBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputActionValue.h"
#include "Debug/ACT_Debug.h"

void AACT_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    if (!LocalPlayer || !InputMappingContext)
    {
        return;
    }

    UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

    if (Subsystem)
    {
        Subsystem->AddMappingContext(InputMappingContext, 0);
    }
}

void AACT_PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ULocalPlayer* LocalPlayer = GetLocalPlayer();

    if (LocalPlayer && InputMappingContext)
    {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
        if (Subsystem)
        {
            Subsystem->RemoveMappingContext(InputMappingContext);
        }
        else
        {
            ACTDebug::Screen(TEXT("Enhanced Input subsystem not found"), 13, 10.0f, FColor::Red);
        }
    }

    Super::EndPlay(EndPlayReason);
}

void AACT_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    if (!EnhancedInput)
    {
        ACTDebug::Screen(TEXT("Input component is not EnhancedInputComponent"), 16, 10.0f, FColor::Red);
        return;
    }

    if (MoveAction)
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AACT_PlayerController::Move);
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &AACT_PlayerController::StopMove);
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Canceled, this, &AACT_PlayerController::StopMove);
    }
    if (LookAction)
    {
        EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AACT_PlayerController::Look);
    }
    if (LightAttackAction)
    {
        EnhancedInput->BindAction(LightAttackAction, ETriggerEvent::Triggered, this, &AACT_PlayerController::LightAttack);
    }

    if (TargetLockAction)
    {
        EnhancedInput->BindAction(TargetLockAction, ETriggerEvent::Triggered, this, &AACT_PlayerController::ToggleTargetLock);
    }
}

void AACT_PlayerController::Move(const FInputActionValue& Value)
{
    AACT_CharacterBase* ControlledCharacter = Cast<AACT_CharacterBase>(GetPawn());

    if (!ControlledCharacter)
    {
        return;
    }

    const FVector2D Axis = Value.Get<FVector2D>();

    // Use only the controller's yaw so movement stays on the ground plane.
    const FRotator YawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
    const FRotationMatrix Rotation(YawRotation);

    const FVector MoveIntent = Rotation.GetUnitAxis(EAxis::X) * Axis.Y + Rotation.GetUnitAxis(EAxis::Y) * Axis.X;
    ControlledCharacter->SetMoveIntent(MoveIntent);

    ControlledCharacter->MoveInWorldDirection(Rotation.GetUnitAxis(EAxis::X), Axis.Y);
    ControlledCharacter->MoveInWorldDirection(Rotation.GetUnitAxis(EAxis::Y), Axis.X);
}

void AACT_PlayerController::StopMove()
{
    AACT_CharacterBase* ControlledCharacter = Cast<AACT_CharacterBase>(GetPawn());

    if (ControlledCharacter)
    {
        ControlledCharacter->SetMoveIntent(FVector::ZeroVector);
    }
}

void AACT_PlayerController::Look(const FInputActionValue& Value)
{
    AACT_CharacterBase* ControlledCharacter = Cast<AACT_CharacterBase>(GetPawn());
    if (!ControlledCharacter || ControlledCharacter->IsTargetLocked())
    {
        return;
    }

    const FVector2D Axis = Value.Get<FVector2D>();
    AddYawInput(Axis.X);
    AddPitchInput(Axis.Y);
}

void AACT_PlayerController::LightAttack()
{
    AACT_CharacterBase* ControlledCharacter = Cast<AACT_CharacterBase>(GetPawn());

    if (!ControlledCharacter || !LightAttackAbilityTag.IsValid() || !LightAttackInputTag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Light attack failed: character or input tags are invalid."));
        return;
    }

    const bool bAccepted = ControlledCharacter->RequestLightAttack(LightAttackAbilityTag, LightAttackInputTag);

    UE_LOG(LogTemp, Log, TEXT("Light attack input %s."), bAccepted ? TEXT("accepted") : TEXT("not accepted"));
}

void AACT_PlayerController::ToggleTargetLock()
{
    AACT_CharacterBase* ControlledCharacter = Cast<AACT_CharacterBase>(GetPawn());

    if (ControlledCharacter)
    {
        ControlledCharacter->ToggleTargetLock();
    }
}