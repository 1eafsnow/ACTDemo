// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ACT_GameMode.h"
#include "Character/ACT_PlayerCharacter.h"
#include "Controller/ACT_PlayerController.h"

AACT_GameMode::AACT_GameMode()
{
    DefaultPawnClass = AACT_PlayerCharacter::StaticClass();
    PlayerControllerClass = AACT_PlayerController::StaticClass();
}
