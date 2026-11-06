// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ACT_CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Ability/ACT_AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
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

    AbilitySystemComponent = CreateDefaultSubobject<UACT_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));

    ComboComponent = CreateDefaultSubobject<UACT_ComboComponent>(TEXT("ComboComponent"));
}

// Called when the game starts or when spawned
void AACT_CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
    InitializeAbilityActorInfo();
    GrantDefaultAbilities();
}

// Called every frame
void AACT_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AACT_CharacterBase::MoveInWorldDirection(FVector Direction, float Scale)
{
    AddMovementInput(Direction.GetSafeNormal2D(), Scale);
}

void AACT_CharacterBase::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    InitializeAbilityActorInfo();
}

void AACT_CharacterBase::OnRep_Controller()
{
    Super::OnRep_Controller();

    InitializeAbilityActorInfo();
}

UAbilitySystemComponent* AACT_CharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AACT_CharacterBase::InitializeAbilityActorInfo()
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

void AACT_CharacterBase::GrantDefaultAbilities()
{
    if (!HasAuthority() || !AbilitySystemComponent || bDefaultAbilitiesGranted)
    {
        return;
    }

    bDefaultAbilitiesGranted = true;

    for (const TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilities)
    {
        if (!AbilityClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s has an empty default ability entry"), *GetNameSafe(this));
            continue;
        }

        AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));
    }
}

bool AACT_CharacterBase::TryActivateAbilityByTag(FGameplayTag AbilityTag)
{
    if (!AbilitySystemComponent || !AbilityTag.IsValid())
    {
        return false;
    }

    return AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag));
}