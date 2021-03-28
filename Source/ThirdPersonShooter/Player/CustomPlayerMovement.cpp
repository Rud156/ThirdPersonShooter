// Fill out your copyright notice in the Description page of Project Settings.


#include "./CustomPlayerMovement.h"

#include "GameFramework/Character.h"

UCustomPlayerMovement::UCustomPlayerMovement(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UCustomPlayerMovement::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
}

FNetworkPredictionData_Client* UCustomPlayerMovement::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		UCustomPlayerMovement* MutableThis = const_cast<UCustomPlayerMovement*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_CustomMovement(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UCustomPlayerMovement::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
}

void FSavedMove_CustomMovement::Clear()
{
	Super::Clear();
}

uint8 FSavedMove_CustomMovement::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	return Result;
}

bool FSavedMove_CustomMovement::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void FSavedMove_CustomMovement::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UCustomPlayerMovement* CharMov = Cast<UCustomPlayerMovement>(C->GetCharacterMovement());
	if (CharMov)
	{
	}
}

void FSavedMove_CustomMovement::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	UCustomPlayerMovement* CharMov = Cast<UCustomPlayerMovement>(C->GetCharacterMovement());
	if (CharMov)
	{
	}
}

FNetworkPredictionData_Client_CustomMovement::FNetworkPredictionData_Client_CustomMovement(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_CustomMovement::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_CustomMovement());
}
