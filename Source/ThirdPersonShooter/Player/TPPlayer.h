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
	FVector2D _meshLocation; // X: Target, Y: Current
	float _capsuleLerpAmount;
	void SetCapsuleData(const float TargetHeight, const float TargetRadius, const float MeshTargetPosition);
	void UpdateCapsuleSize(const float DeltaTime);

	bool _lastFrameFalling;
	bool _isAdsBeforeFalling;
	void UpdateFalling(const float DeltaTime);

	bool _isLeftShoulder;
	bool _isInAds;
	float _shoulderCameraLerpAmount;
	FVector _shoulderStartPosition;
	FVector _shoulderEndPosition;
	FVector2D _cameraBoomLength; // X: Target, Y: Current
	void UpdateShoulderCamera(const float DeltaTime);

	bool CanAcceptADSInput() const;

	FVector _diveDirection;
	FRotator _diveStartRotation;
	FRotator _diveEndRotation;
	float _diveLerpAmount;
	bool _acceptDiveInput;
	void UpdateDive(const float DeltaTime);

	FRotator _runStartRotation;
	FRotator _runEndRotation;
	float _runLerpAmount;
	void UpdateRunMeshRotation(const float DeltaTime);
	void ResetPreRunRotation(const bool ForceReset = false);

	bool _isRunningBeforeTrace;
	bool _isJumpPressed;
	bool _isClimbing;
	FHitResult _forwardTrace;
	FHitResult _heightTrace;
	FHitResult _forwardHeightTrace;
	bool WallClimbForwardTrace();
	bool WallClimbHeightTrace();
	bool VaultForwardHeightTrace();
	bool HandleWallClimb();
	bool HandleVault();
	void UpdateWallClimbCheck(const float DeltaTime);

	void MoveForward(const float Value);
	void MoveRight(const float Value);
	void TurnAtRate(const float Value);
	void LookUpRate(const float Value);
	void HandleJumpPressed();
	void HandleJumpReleased();
	void HandleSprintPressed();
	void HandleCrouchPressed();
	void HandleShoulderSwapPressed();
	void HandleDivePressed();
	void HandleADSPressed();
	bool CanAcceptPlayerInput() const;

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
	float RunSpeed;

	UPROPERTY(Category = "Player|Movement", EditAnywhere)
	float RunLerpSpeed;

	UPROPERTY(Category = "Player|Movement", EditAnywhere)
	float DiveSpeed;

	UPROPERTY(Category = "Player|Movement", EditAnywhere)
	float DiveGetUpSpeed;

	UPROPERTY(Category = "Player|Movement", EditAnywhere)
	float DiveLerpSpeed;

	UPROPERTY(Category = "Player|Camera", EditAnywhere)
	FVector CameraLeftShoulder;

	UPROPERTY(Category = "Player|Camera", EditAnywhere)
	FVector CameraADSLeftShoulder;

	UPROPERTY(Category = "Player|Camera", EditAnywhere)
	FVector CameraRightShoulder;

	UPROPERTY(Category = "Player|Camera", EditAnywhere)
	FVector CameraADSRightShoulder;

	UPROPERTY(Category = "Player|Camera", EditAnywhere)
	float CameraDefaultBoomLength;

	UPROPERTY(Category = "Player|Camera", EditAnywhere)
	float CameraADSBoomLength;

	UPROPERTY(Category = "Player|Camera", EditAnywhere)
	FVector RunCameraLeftPosition;

	UPROPERTY(Category = "Player|Camera", EditAnywhere)
	FVector RunCameraRightPosition;

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

	UPROPERTY(Category = "Player|Size", EditAnywhere)
	float DefaultMeshZPosition;

	UPROPERTY(Category = "Player|Size", EditAnywhere)
	float CrouchMeshZPosition;

	UPROPERTY(Category = "Player|Size", EditAnywhere)
	float MeshDefaultZRotation;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	float WallClimbForwardCheck;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	float WallClimbHeightForwardCheck;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	FVector WallClimbUpOffset;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	FVector WallClimbUpDownOffset;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	FVector VaultDownOffset;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	float VaultDownZDiffOffset;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	float ClimbAnimXOffset;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	float ClimbAnimZOffset;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	float VaultAnimXOffset;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	float VaultAnimZOffset;

	UPROPERTY(Category = "Player|WallClimb", EditAnywhere)
	float WallClimbHeight;

	UPROPERTY(Category = "Player|WallClimb", EditAnywhere)
	float WallClimbMinHeight;

	UPROPERTY(Category = "Player|WallClimb", EditAnywhere)
	float VaultWallHeight;

	UPROPERTY(Category = "Player|WallClimb", EditAnywhere)
	float VaultWallMinHeight;

	UPROPERTY(Category = "Player|WallClimb", EditAnywhere)
	float VaultThicknessDistance;

	UPROPERTY(Category = "Player|WallClimb", BlueprintReadOnly)
	float VaultDownDistance;

	UPROPERTY(Category = "Player|WallClimb", EditAnywhere)
	FName WallClimbableTag;

#pragma endregion

	ATPPlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Category = "Player|Movement", BlueprintImplementableEvent)
	void PlayerJumpNotify();

	UFUNCTION(Category = "Player|Movement", BlueprintImplementableEvent)
	void PlayerDiveNotify(const bool UseCrouchAnimation);

	UFUNCTION(Category = "Player|Movement", BlueprintImplementableEvent)
	void ShoulderSwapNotify();

	UFUNCTION(Category = "Player|Movement", BlueprintImplementableEvent)
	void PlayerClimbNotify(FRotator TargetRotation, FVector Delta);

	UFUNCTION(Category = "Player|Movement", BlueprintImplementableEvent)
	void PlayerVaultNotify(FRotator TargetRotation, FVector Delta);

	UFUNCTION(Category = "Player|Movement", BlueprintCallable)
	void HandleClimbAnimComplete();

	UFUNCTION(Category = "Player|Movement", BlueprintCallable)
	void HandleVaultAnimComplete();

	UFUNCTION(Category = "Player|Movement", BlueprintCallable)
	void HandleDiveAnimComplete();

	UFUNCTION(Category = "Player|Movement", BlueprintCallable)
	void HandleDiveResetAngle();

	UFUNCTION(Category = "Player|Movement", BlueprintCallable, BlueprintPure)
	EPlayerMovementState GetTopPlayerState() const;

	UFUNCTION(Category = "Player|Movement", BlueprintCallable, BlueprintPure)
	float GetVerticalInput() const;

	UFUNCTION(Category = "Player|Movement", BlueprintCallable, BlueprintPure)
	float GetHorizontalInput() const;

	UFUNCTION(Category = "Player|Movement", BlueprintCallable, BlueprintPure)
	bool IsRunning();

	UFUNCTION(Category = "Player|Camera", BlueprintCallable, BlueprintPure)
	bool IsInAds();

	UFUNCTION(Category = "Player|Camera", BlueprintCallable, BlueprintPure)
	bool IsLeftShoulder();
};
