// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "ACT_ComboData.h"
#include "ACT_ComboComponent.generated.h"

/** Sent when an attack node needs to be validated and played. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FACT_OnAttackRequested, const FACT_ComboNode&, AttackNode);

/** Sent when an active combo ends or is cancelled. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FACT_OnComboEnded);

/**
 * Tracks combo state and resolves transitions between attack nodes.
 */
UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class ACTDEMO_API UACT_ComboComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACT_ComboComponent();

    /**
     * Starts a combo by requesting an entry node.
     * The attack remains pending until ConfirmAttack is called.
     */
    UFUNCTION(BlueprintCallable, Category = "Combo")
    bool StartCombo(UACT_ComboData* ComboData, FGameplayTag InputTag);

    /**
     * Stores the latest input while the combo input window is open.
     */
    UFUNCTION(BlueprintCallable, Category = "Combo")
    bool SubmitInput(FGameplayTag InputTag);

    UFUNCTION(BlueprintCallable, Category = "Combo")
    void OpenInputWindow();

    UFUNCTION(BlueprintCallable, Category = "Combo")
    void CloseInputWindow();

    /**
     * Resolves the buffered input and requests the next attack.
     */
    UFUNCTION(BlueprintCallable, Category = "Combo")
    bool CommitTransition();

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
    FACT_OnAttackRequested OnAttackRequested;

    UPROPERTY(BlueprintAssignable, Category = "Combo")
    FACT_OnComboEnded OnComboEnded;

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    const FACT_ComboNode* FindNode(FName NodeId) const;
    FName FindEntryNode(FGameplayTag InputTag) const;
    FName FindNextNode(FGameplayTag InputTag) const;
    bool RequestAttack(FName NodeId);
    void ResetState();

    UPROPERTY(Transient)
    TObjectPtr<UACT_ComboData> ActiveComboData = nullptr;

    FName CurrentNodeId = NAME_None;
    FName PendingNodeId = NAME_None;
    FGameplayTag BufferedInput;
    bool bInputWindowOpen = false;

};
