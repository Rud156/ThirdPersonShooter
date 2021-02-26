// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Utils/Enums.h"
#include "TPPlayer.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API ATPPlayer : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	float _horizontalInput;
	float _verticalInput;

	TArray<EPlayerMovementState> _movementStack;
	void PushPlayerMovementState(const EPlayerMovementState MovementState);
	void RemovePlayerMovementState(const EPlayerMovementState MovementState);
	bool HasPlayerState(const EPlayerMovementState MovementState);
	void ApplyChangesToCharacter();

	FVector2D _capsuleRadius; // X: Target, Y: Current
	FVector2D _capsuleHeight; // X: Target, Y: Current
	float _capsuleLerpAmount;
	void SetCapsuleData(const float TargetHeight, const float TargetRadius);
	void UpdateCapsuleSize(const float DeltaTime);

	bool _isLeftShoulder;
	bool _isInAds;
	float _shoulderCameraLerpAmount;
	FVector _shoulderStartPosition;
	FVector _shoulderEndPosition;
	void UpdateShoulderCamera(const float DeltaTime);

	float _diveTimer;
	FVector _diveDirection;
	void UpdateDive(const float DeltaTime);

	void MoveForward(const float Value);
	void MoveRight(const float Value);
	void TurnAtRate(const float Value);
	void LookUpRate(const float Value);
	void JumpCharacter();
	void HandleSprintPressed();
	void HandleCrouchPressed();
	void HandleShoulderSwapPressed();
	void HandleDivePressed();
	void HandleADSPressed();

protected:
	virtual void BeginPlay() override;

public:
#pragma region Properties

	UPROPERTY(Category = "Player|Movement", EditAnywhere)
		float BaseLookUpRate;

	UPROPERTY(Category = "Player|Movement", EditAnywhere)
		float BaseTurnRate;

	UPROPERTY(Category = "Player|Movement", EditAnywhere)
		float CrouchSpeed;

	UPROPERTY(Category = "Player|Movement", EditAnywhere)
		float WalkSpeed;

	UPROPERTY(Category = "Player|Movement", EditAnywhere)
		float SprintSpeed;

	UPROPERTY(Category = "Player|Movement", EditAnywhere)
		float DiveSpeed;

	UPROPERTY(Category = "Player|Movement", EditAnywhere)
		float DiveDuration;

	UPROPERTY(Category = "Player|Camera", EditAnywhere)
		FVector CameraLeftShoulder;

	UPROPERTY(Category = "Player|Camera", EditAnywhere)
		FVector CameraADSLeftShoulder;

	UPROPERTY(Category = "Player|Camera", EditAnywhere)
		FVector CameraRightShoulder;

	UPROPERTY(Category = "Player|Camera", EditAnywhere)
		FVector CameraADSRightShoulder;

	UPROPERTY(Category = "Player|Camera", EditAnywhere)
		float CameraLerpSpeed;

	UPROPERTY(Category = "Player|Size", EditAnywhere)
		float DefaultHalfHeight;

	UPROPERTY(Category = "Player|Size", EditAnywhere)
		float DefaultRadius;

	UPROPERTY(Category = "Player|Size", EditAnywhere)
		float CrouchHalfHeight;

	UPROPERTY(Category = "Player|Size", EditAnywhere)
		float CrouchRadius;

	UPROPERTY(Category = "Player|Size", EditAnywhere)
		float CapsuleSizeLerpRate;

#pragma endregion

	ATPPlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Category = "Player|Movement", BlueprintCallable, BlueprintPure)
		EPlayerMovementState GetTopPlayerState() const;

	UFUNCTION(Category = "Player|Movement", BlueprintCallable, BlueprintPure)
		float GetVerticalInput() const;

	UFUNCTION(Category = "Player|Movement", BlueprintCallable, BlueprintPure)
		float GetHorizontalInput() const;
};
