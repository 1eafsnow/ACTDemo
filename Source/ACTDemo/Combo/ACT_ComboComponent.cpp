// Fill out your copyright notice in the Description page of Project Settings.


#include "Combo/ACT_ComboComponent.h"

// Sets default values for this component's properties
UACT_ComboComponent::UACT_ComboComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UACT_ComboComponent::StartCombo(UACT_ComboData* ComboData, FGameplayTag InputTag, FACT_ComboNode& OutAttackNode)
{
    OutAttackNode = FACT_ComboNode();

    if (IsComboActive() || !IsValid(ComboData) || !InputTag.IsValid())
    {
        return false;
    }

    ActiveComboData = ComboData;

    const FName EntryNodeId = FindEntryNode(InputTag);
    const FACT_ComboNode* EntryNode = FindNode(EntryNodeId);

    if (!EntryNode || !EntryNode->Montage)
    {
        ResetState();
        return false;
    }

    PendingNodeId = EntryNodeId;
    OutAttackNode = *EntryNode;
    return true;
}

bool UACT_ComboComponent::SubmitInput(FGameplayTag InputTag)
{
    if (!IsComboActive() || !bInputWindowOpen || !PendingNodeId.IsNone() || !InputTag.IsValid())
    {
        return false;
    }

    BufferedInput = InputTag;
    return true;
}

void UACT_ComboComponent::OpenInputWindow()
{
    if (!IsComboActive() || CurrentNodeId.IsNone() || !PendingNodeId.IsNone())
    {
        return;
    }

    bInputWindowOpen = true;
}

void UACT_ComboComponent::CloseInputWindow()
{
    bInputWindowOpen = false;
    BufferedInput = FGameplayTag();
}

bool UACT_ComboComponent::CommitTransition(FACT_ComboNode& OutAttackNode)
{
    OutAttackNode = FACT_ComboNode();

    if (!IsComboActive() || !bInputWindowOpen || !BufferedInput.IsValid() || !PendingNodeId.IsNone())
    {
        return false;
    }

    const FName NextNodeId = FindNextNode(BufferedInput);

    BufferedInput = FGameplayTag();
    bInputWindowOpen = false;

    const FACT_ComboNode* NextNode = FindNode(NextNodeId);

    if (!NextNode || !NextNode->Montage)
    {
        return false;
    }

    PendingNodeId = NextNodeId;
    OutAttackNode = *NextNode;
    return true;
}

bool UACT_ComboComponent::ConfirmAttack(bool bAccepted)
{
    if (!IsComboActive() || PendingNodeId.IsNone())
    {
        return false;
    }

    if (!bAccepted)
    {
        EndCombo();
        return false;
    }

    CurrentNodeId = PendingNodeId;
    PendingNodeId = NAME_None;
    BufferedInput = FGameplayTag();
    bInputWindowOpen = false;
    return true;
}

void UACT_ComboComponent::EndCombo()
{
    if (!IsComboActive())
    {
        return;
    }

    ResetState();
    OnComboEnded.Broadcast();
}

bool UACT_ComboComponent::IsComboActive() const
{
    return ActiveComboData != nullptr;
}

bool UACT_ComboComponent::IsInputWindowOpen() const
{
    return bInputWindowOpen;
}

FName UACT_ComboComponent::GetCurrentNodeId() const
{
    return CurrentNodeId;
}

void UACT_ComboComponent::NotifyAttackStarted()
{
    const FACT_ComboNode* CurrentNode = FindNode(CurrentNodeId);

    if (CurrentNode)
    {
        OnAttackStarted.Broadcast(*CurrentNode);
    }
}

void UACT_ComboComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ResetState();
    Super::EndPlay(EndPlayReason);
}

const FACT_ComboNode* UACT_ComboComponent::FindNode(FName NodeId) const
{
    if (!ActiveComboData || NodeId.IsNone())
    {
        return nullptr;
    }

    return ActiveComboData->Nodes.FindByPredicate([NodeId](const FACT_ComboNode& Node) { return Node.NodeId == NodeId; });
}

FName UACT_ComboComponent::FindEntryNode(FGameplayTag InputTag) const
{
    if (!ActiveComboData)
    {
        return NAME_None;
    }

    const FACT_ComboEntry* Entry = ActiveComboData->EntryPoints.FindByPredicate([InputTag](const FACT_ComboEntry& Item) { return Item.InputTag == InputTag; });

    return Entry ? Entry->NodeId : NAME_None;
}

FName UACT_ComboComponent::FindNextNode(FGameplayTag InputTag) const
{
    const FACT_ComboNode* CurrentNode = FindNode(CurrentNodeId);

    if (!CurrentNode)
    {
        return NAME_None;
    }

    const FACT_ComboTransition* Transition = CurrentNode->Transitions.FindByPredicate([InputTag](const FACT_ComboTransition& Item) { return Item.InputTag == InputTag; });

    return Transition ? Transition->NextNodeId : NAME_None;
}

void UACT_ComboComponent::ResetState()
{
    ActiveComboData = nullptr;
    CurrentNodeId = NAME_None;
    PendingNodeId = NAME_None;
    BufferedInput = FGameplayTag();
    bInputWindowOpen = false;
}

