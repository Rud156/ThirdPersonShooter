// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomPlayerMovement.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API UCustomPlayerMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UCustomPlayerMovement(const FObjectInitializer& ObjectInitializer);

	friend class FSavedMove_ExtendedCustomMovement;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
};

class FSavedMove_CustomMovement : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;

	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(ACharacter* C) override;
};

class FNetworkPredictionData_Client_CustomMovement : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_CustomMovement(const UCharacterMovementComponent& ClientMovement);
	typedef FNetworkPredictionData_Client_Character Super;

	virtual FSavedMovePtr AllocateNewMove() override;
};
