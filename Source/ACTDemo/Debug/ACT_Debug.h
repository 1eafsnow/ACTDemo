#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"

/** Helper functions for development debugging. */
class ACTDEMO_API ACTDebug
{
public:
    /** Prints a message on screen. Use the same key to update a message. */
    static void Screen(const FString& Message, int32 Key = -1, float Duration = 3.0f, FColor Color = FColor::Yellow)
    {
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(Key, Duration, Color, Message);
        }
#endif
    }

    /** Writes a message to the Output Log. */
    static void Log(const FString& Message)
    {
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
        UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
#endif
    }
};