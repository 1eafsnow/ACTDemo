// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "ACT_ComboData.h"
#include "TimerManager.h"
#include "ACT_ComboComponent.generated.h"

/** Sent after an attack montage has started successfully. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FACT_OnAttackStarted, const FACT_ComboNode&, AttackNode);

/** Sent when an active combo ends or is cancelled. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FACT_OnComboEnded);

/**
 * Tracks combo state and resolves transitions between attack nodes.
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class ACTDEMO_API UACT_ComboComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACT_ComboComponent();

    /** Finds the entry attack and marks it as pending confirmation. */
    UFUNCTION(BlueprintCallable, Category = "Combo")
    bool StartCombo(UACT_ComboData* ComboData, FGameplayTag InputTag, FACT_ComboNode& OutAttackNode);

    /**
     * Stores the latest input while the combo input window is open.
     */
    UFUNCTION(BlueprintCallable, Category = "Combo")
    bool SubmitInput(FGameplayTag InputTag);

    UFUNCTION(BlueprintCallable, Category = "Combo")
    void OpenInputWindow();

    UFUNCTION(BlueprintCallable, Category = "Combo")
    void CloseInputWindow();

    /** Resolves buffered input and returns the next attack pending confirmation. */
    UFUNCTION(BlueprintCallable, Category = "Combo")
    bool CommitTransition(FACT_ComboNode& OutAttackNode);

    /**
     * Accepts or rejects the currently pending attack.
     */
    UFUNCTION(BlueprintCallable, Category = "Combo")
    bool ConfirmAttack(bool bAccepted);

    /**
     * Clears combo state and broadcasts OnComboEnded once.
     */
    UFUNCTION(BlueprintCallable, Category = "Combo")
    void EndCombo();

    UFUNCTION(BlueprintPure, Category = "Combo")
    bool IsComboActive() const;

    UFUNCTION(BlueprintPure, Category = "Combo")
    bool IsInputWindowOpen() const;

    UFUNCTION(BlueprintPure, Category = "Combo")
    FName GetCurrentNodeId() const;

    UPROPERTY(BlueprintAssignable, Category = "Combo")
    FACT_OnComboEnded OnComboEnded;

    /** Notifies listeners that the confirmed attack has started playing. */
    UFUNCTION(BlueprintCallable, Category = "Combo")
    void NotifyAttackStarted();

    UPROPERTY(BlueprintAssignable, Category = "Combo")
    FACT_OnAttackStarted OnAttackStarted;

    /** Saves the current combo node for a short time after movement cancellation. */
    void StartResumeWindow();

    /** Clears any saved combo continuation. */
    void ClearResumeWindow();

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // Time allowed to continue a combo after movement cancellation.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo", meta = (ClampMin = "0.0"))
    float ResumeWindowDuration = 0.5f;

private:
    const FACT_ComboNode* FindNode(FName NodeId) const;
    FName FindEntryNode(FGameplayTag InputTag) const;
    FName FindNextNode(FGameplayTag InputTag) const;
    void ResetState();

    UPROPERTY(Transient)
    TObjectPtr<UACT_ComboData> ActiveComboData = nullptr;

    FName CurrentNodeId = NAME_None;
    FName PendingNodeId = NAME_None;
    FGameplayTag BufferedInput;
    bool bInputWindowOpen = false;

    UPROPERTY(Transient)
    TObjectPtr<UACT_ComboData> ResumeComboData = nullptr;

    FName ResumeNodeId = NAME_None;
    FTimerHandle ResumeTimerHandle;

};
