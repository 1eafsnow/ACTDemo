// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "ACT_CharacterBase.generated.h"

class UACT_AbilitySystemComponent;
class UGameplayAbility;
class UACT_ComboComponent;

/**
 * Common base class for player and enemy characters.
 */
UCLASS()
class ACTDEMO_API AACT_CharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UACT_ComboComponent> ComboComponent;

public:
	// Sets default values for this character's properties
	AACT_CharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * Applies movement input along a world-space direction.
	 *
	 * @param Direction Movement direction in world space.
	 * @param Scale Input magnitude.
	 */
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void MoveInWorldDirection(FVector Direction, float Scale = 1.0f);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Attempts to activate granted abilities matching the supplied tag. */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTag);

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;

	/** Abilities granted once during character initialization. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UACT_AbilitySystemComponent> AbilitySystemComponent;

private:
	void InitializeAbilityActorInfo();
	void GrantDefaultAbilities();

	bool bDefaultAbilitiesGranted = false;

};
