// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ACT_ComboData.generated.h"

class UAnimMontage;

/**
 * Defines a transition from the current attack to another attack.
 */
USTRUCT(BlueprintType)
struct FACT_ComboTransition
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    FGameplayTag InputTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    FName NextNodeId = NAME_None;
};

/**
 * Defines one attack in a combo.
 */
USTRUCT(BlueprintType)
struct FACT_ComboNode
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    FName NodeId = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    TObjectPtr<UAnimMontage> Montage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    FName SectionName = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo", meta = (ClampMin = "0.0"))
    float StaminaCost = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    TArray<FACT_ComboTransition> Transitions;
};

/**
 * Maps an initial input to the first attack in a combo.
 */
USTRUCT(BlueprintType)
struct FACT_ComboEntry
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    FGameplayTag InputTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    FName NodeId = NAME_None;
};

/**
 * Shared configuration for a combo.
 */
UCLASS(BlueprintType)
class ACTDEMO_API UACT_ComboData : public UDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    TArray<FACT_ComboEntry> EntryPoints;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    TArray<FACT_ComboNode> Nodes;
	
};
