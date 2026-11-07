// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ACT_AbilitySystemComponent.generated.h"

/**
 * Ability system component shared by ACT characters.
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = (Abilities), meta = (BlueprintSpawnableComponent))
class ACTDEMO_API UACT_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	UACT_AbilitySystemComponent();
};
