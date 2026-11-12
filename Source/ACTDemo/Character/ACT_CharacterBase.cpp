// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ACT_CharacterBase.h"
#include "Debug/ACT_Debug.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Ability/ACT_AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Combo/ACT_ComboComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Controller.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Weapon/ACT_EquipmentComponent.h"

// Sets default values
AACT_CharacterBase::AACT_CharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Rotate toward the movement direction by default.
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

    AbilitySystemComponent = CreateDefaultSubobject<UACT_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));

    ComboComponent = CreateDefaultSubobject<UACT_ComboComponent>(TEXT("ComboComponent"));
    EquipmentComponent = CreateDefaultSubobject<UACT_EquipmentComponent>(TEXT("EquipmentComponent"));
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

    if (bHadLockTarget)
    {
        UpdateTargetLock(DeltaTime);
    }

    if (bAttackTurnWindowOpen)
    {
        UpdateAttackTurn(DeltaTime);
    }
}

void AACT_CharacterBase::MoveInWorldDirection(FVector Direction, float Scale)
{
    if (ComboComponent && ComboComponent->IsComboActive())
    {
        return;
    }

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

void AACT_CharacterBase::ToggleTargetLock()
{
    if (bHadLockTarget)
    {
        UnlockTarget();
        return;
    }

    AActor* NewTarget = FindLockTarget();

    if (!NewTarget || !Controller)
    {
        ACTDebug::Screen(TEXT("FindLockTarget Failed"));
        return;
    }

    LockTarget = NewTarget;
    bHadLockTarget = true;

    bPreviousOrientRotationToMovement = GetCharacterMovement()->bOrientRotationToMovement;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    SetActorTickEnabled(true);
}

void AACT_CharacterBase::UnlockTarget()
{
    LockTarget.Reset();
    bHadLockTarget = false;

    GetCharacterMovement()->bOrientRotationToMovement = bPreviousOrientRotationToMovement;

    SetActorTickEnabled(bAttackTurnWindowOpen);
}

bool AACT_CharacterBase::IsTargetLocked() const
{
    return bHadLockTarget && LockTarget.IsValid();
}

AActor* AACT_CharacterBase::GetLockTarget() const
{
    return LockTarget.Get();
}

AActor* AACT_CharacterBase::FindLockTarget() const
{
    if (!Controller || !GetWorld())
    {
        return nullptr;
    }

    const FVector Origin = GetActorLocation();
    const FVector ViewDirection = Controller->GetControlRotation().Vector().GetSafeNormal2D();
    const float MinimumDot = FMath::Cos(FMath::DegreesToRadians(LockSearchHalfAngle));

    AActor* BestTarget = nullptr;
    float BestDistanceSquared = FMath::Square(LockSearchDistance);

    for (TActorIterator<ACharacter> It(GetWorld()); It; ++It)
    {
        ACharacter* Candidate = *It;

        if (!IsValid(Candidate) || Candidate == this || !Candidate->ActorHasTag(LockTargetTag))
        {
            continue;
        }

        const FVector Offset = Candidate->GetActorLocation() - Origin;
        const float DistanceSquared = Offset.SizeSquared();

        if (DistanceSquared > BestDistanceSquared)
        {
            continue;
        }

        const FVector Direction = Offset.GetSafeNormal2D();

        if (Direction.IsNearlyZero() || FVector::DotProduct(ViewDirection, Direction) < MinimumDot)
        {
            continue;
        }

        BestTarget = Candidate;
        BestDistanceSquared = DistanceSquared;
    }

    return BestTarget;
}

void AACT_CharacterBase::UpdateTargetLock(float DeltaTime)
{
    AActor* Target = LockTarget.Get();

    if (!IsValid(Target) || !Controller || FVector::DistSquared(GetActorLocation(), Target->GetActorLocation()) > FMath::Square(LockBreakDistance))
    {
        UnlockTarget();
        return;
    }

    const FVector TargetOffset = Target->GetActorLocation() - GetActorLocation();
    const FRotator TargetRotation = TargetOffset.Rotation();

    const FRotator CurrentControlRotation = Controller->GetControlRotation();
    const FRotator TargetCameraRotation(TargetRotation.Pitch - LockCameraDownPitch, TargetRotation.Yaw, 0.0f);
    const FRotator NewControlRotation = FMath::RInterpTo(CurrentControlRotation, TargetCameraRotation, DeltaTime, LockCameraRotationSpeed);
    Controller->SetControlRotation(NewControlRotation);

    const FRotator CurrentCharacterRotation = GetActorRotation();
    const FRotator TargetCharacterRotation(0.0f, TargetRotation.Yaw, 0.0f);
    const FRotator NewCharacterRotation = FMath::RInterpTo(CurrentCharacterRotation, TargetCharacterRotation, DeltaTime, LockCharacterRotationSpeed);
    SetActorRotation(NewCharacterRotation);
}

bool AACT_CharacterBase::RequestLightAttack(FGameplayTag AbilityTag, FGameplayTag InputTag)
{
    if (!ComboComponent || !AbilityTag.IsValid() || !InputTag.IsValid())
    {
        return false;
    }

    if (ComboComponent->IsComboActive())
    {
        if (!ComboComponent->SubmitInput(InputTag))
        {
            return false;
        }

        CaptureAttackDirection();

        const FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combo.Input"));

        FGameplayEventData EventData;
        EventData.EventTag = EventTag;
        EventData.Instigator = this;

        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
        return true;
    }

    CaptureAttackDirection();
    return TryActivateAbilityByTag(AbilityTag);
}

void AACT_CharacterBase::SetMoveIntent(const FVector& Direction)
{
    const bool bPreviouslyHadInput = HasMoveIntent();

    MoveIntent = Direction.GetClampedToMaxSize2D(1.0f);

    if (!bPreviouslyHadInput && HasMoveIntent())
    {
        FGameplayEventData EventData;
        EventData.EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Move.Input"));
        EventData.Instigator = this;

        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
    }
}

bool AACT_CharacterBase::HasMoveIntent() const
{
    constexpr float InputThreshold = 0.1f;
    return MoveIntent.SizeSquared2D() > FMath::Square(InputThreshold);
}

void AACT_CharacterBase::SetAttackTurnWindow(bool bOpen)
{
    bAttackTurnWindowOpen = bOpen;

    SetActorTickEnabled(bHadLockTarget || bAttackTurnWindowOpen);
}

bool AACT_CharacterBase::IsAttackTurnWindowOpen() const
{
    return bAttackTurnWindowOpen;
}

void AACT_CharacterBase::UpdateAttackTurn(float DeltaTime)
{
    if (!bAttackTurnWindowOpen || IsTargetLocked() || AttackDirection.IsNearlyZero())
    {
        return;
    }

    const FRotator TargetRotation(0.0f, AttackDirection.Rotation().Yaw, 0.0f);
    const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, AttackTurnSpeed);

    SetActorRotation(NewRotation);
}

void AACT_CharacterBase::CaptureAttackDirection()
{
    AttackDirection = MoveIntent.IsNearlyZero() ? GetActorForwardVector().GetSafeNormal2D() : MoveIntent.GetSafeNormal2D();
}

UACT_EquipmentComponent* AACT_CharacterBase::GetEquipmentComponent() const
{
    return EquipmentComponent;
}