// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ACT_CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Combo/ACT_ComboComponent.h"

// Sets default values
AACT_CharacterBase::AACT_CharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Rotate toward the movement direction by default.
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

    ComboComponent = CreateDefaultSubobject<UACT_ComboComponent>(TEXT("ComboComponent"));
}

// Called when the game starts or when spawned
void AACT_CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AACT_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AACT_CharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AACT_CharacterBase::MoveInWorldDirection(FVector Direction, float Scale)
{
    AddMovementInput(Direction.GetSafeNormal2D(), Scale);
}