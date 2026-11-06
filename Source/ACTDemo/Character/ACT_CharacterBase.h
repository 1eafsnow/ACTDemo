// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ACT_CharacterBase.generated.h"

/**
 * Common base class for player and enemy characters.
 */
UCLASS()
class ACTDEMO_API AACT_CharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AACT_CharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**
	 * Applies movement input along a world-space direction.
	 *
	 * @param Direction Movement direction in world space.
	 * @param Scale Input magnitude.
	 */
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void MoveInWorldDirection(FVector Direction, float Scale = 1.0f);

};
