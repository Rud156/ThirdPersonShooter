// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Utils/Enums.h"
#include "TPPlayer.generated.h"

class ABaseShootingWeapon;

UCLASS()
class THIRDPERSONSHOOTER_API ATPPlayer : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* WeaponAttachPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* InteractCastPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UHealthAndDamageComponent* HealthAndDamage;

	float _horizontalInput;
	float _verticalInput;

	void PushPlayerMovementState(const EPlayerMovementState MovementState);
	void RemovePlayerMovementState(const EPlayerMovementState MovementState);
	bool HasPlayerState(const EPlayerMovementState MovementState);
	bool IsMoving() const;
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

	EPlayerMovementState _preVJMovementState;
	bool _isJumpPressed;
	FHitResult _forwardTrace;
	FHitResult _heightTrace;
	FHitResult _forwardHeightTrace;
	void WallClimbForwardTrace(bool& CanClimb, bool& CanVault);
	void WallClimbHeightTrace(bool& CanClimb, bool& CanVault);
	bool VaultForwardHeightTrace();
	bool HandleWallClimb();
	bool HandleVault();
	bool CheckAndActivateWallClimb();

	bool _updateVaultForward;
	FVector2D _vaultEndOffset; // X: Target, Y: Current
	float _vaultLerpAmount;
	void UpdateVaultForward(const float DeltaTime);

	FVector2D _startRecoilOffset;
	FVector2D _targetRecoilOffset;
	FVector2D _preRecoilOffset;
	float _recoilLerpAmount;
	bool _resetRecoil;
	bool _firePressed;
	void UpdateRecoilCamera(const float DeltaTime);
	void UpdateFirePressed(const float DeltaTime);
	void BulletShot(const FVector StartPosition, const FVector EndPosition) const;
	void CheckAndDealDamage(AActor* HitActor) const;
	void PickupWeapon(ABaseShootingWeapon* Weapon);
	void DropWeapon(ABaseShootingWeapon* Weapon);

	bool CanAcceptShootingInput() const;
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
	float ClimbAnimXOffset;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	float ClimbAnimZOffset;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	FVector VaultDownOffset;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	float VaultAnimXOffset;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	float VaultAnimZOffset;

	UPROPERTY(Category="Player|NM_WallClimb", EditAnywhere)
	float VaultMoveDownMaxOffset;

	UPROPERTY(Category = "Player|NM_WallClimb", EditAnywhere)
	float VaultDownZDiffOffset;

	UPROPERTY(Category="Player|NM_WallClimb", EditAnywhere)
	float VaultLerpSpeed;

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

	UPROPERTY(Category = "Player|WallClimb", EditAnywhere)
	FName WallClimbableTag;

	UPROPERTY(Category = "Player|WallClimb", EditAnywhere)
	FName VaultTag;

	UPROPERTY(Category = "Player|Attachment", EditAnywhere)
	FName LeftHandSocket;

	UPROPERTY(Category = "Player|Attachment", EditAnywhere)
	FVector LeftAttachmentLocation;

	UPROPERTY(Category = "Player|Attachment", EditAnywhere)
	FRotator LeftAttachmentRotation;

	UPROPERTY(Category = "Player|Attachment", EditAnywhere)
	FName RightHandSocket;

	UPROPERTY(Category = "Player|Attachment", EditAnywhere)
	FVector RightAttachmentLocation;

	UPROPERTY(Category = "Player|Attachment", EditAnywhere)
	FRotator RightAttachmentRotation;

	UPROPERTY(Category = "Player|Interaction", EditAnywhere)
	float InteractionDistance;

	UPROPERTY(Category="Player|Weapon", EditAnywhere)
	float MaxShootDistance;

	UPROPERTY(Category="Player|Weapon", EditAnywhere)
	float RecoilLerpSpeed;

	UPROPERTY(Category="Player|Weapon", EditAnywhere)
	float RecoilCameraMultiplier;

#pragma endregion

#pragma region Networked Data

	UPROPERTY(ReplicatedUsing=OnDataFromNetwork)
	TArray<EPlayerMovementState> N_MovementStack;

	UPROPERTY(ReplicatedUsing=OnDataFromNetwork)
	bool N_IsCameraLeftShoulder;

	UPROPERTY(ReplicatedUsing=OnDataFromNetwork)
	bool N_IsCameraInAds;

	UPROPERTY(ReplicatedUsing=OnDataFromNetwork)
	bool N_IsClimbing;

	UPROPERTY(ReplicatedUsing=OnWeaponDataFromNetwork)
	ABaseShootingWeapon* N_CurrentWeapon;

	UFUNCTION()
	void OnDataFromNetwork();

	UFUNCTION()
	void OnWeaponDataFromNetwork(ABaseShootingWeapon* PreviousWeapon);

#pragma endregion

#pragma region Controller Input

	void MoveForward(const float Value);
	void Client_MoveForward(const float Value);
	UFUNCTION(Server, Reliable)
	void Server_MoveForward(const float Value);
	UFUNCTION(NetMulticast, Reliable)
	void Remote_MoveForward(const float Value);

	void MoveRight(const float Value);
	void Client_MoveRight(const float Value);
	UFUNCTION(Server, Reliable)
	void Server_MoveRight(const float Value);
	UFUNCTION(NetMulticast, Reliable)
	void Remote_MoveRight(const float Value);

	void TurnAtRate(const float Value);
	void Client_TurnAtRate(const float Value);
	UFUNCTION(Server, Reliable)
	void Server_TurnAtRate(const float Value);
	UFUNCTION(NetMulticast, Reliable)
	void Remote_TurnAtRate(const float Value);

	void LookUpRate(const float Value);
	void Client_LookUpRate(const float Value);
	UFUNCTION(Server, Reliable)
	void Server_LookUpControlRotation(const FRotator ControlRotation);
	UFUNCTION(NetMulticast, Reliable)
	void Remote_LookUpControlRotation(const FRotator ControlRotation);

	void HandleJumpPressed();
	void Client_HandleJumpPressed();
	UFUNCTION(Server, Reliable)
	void Server_HandleJumpPressed();
	UFUNCTION(NetMulticast, Reliable)
	void Remote_HandleJumpPressed();

	void HandleJumpReleased();
	void Client_HandleJumpReleased();
	UFUNCTION(Server, Reliable)
	void Server_HandleJumpReleased();
	UFUNCTION(NetMulticast, Reliable)
	void Remote_HandleJumpReleased();

	void HandleSprintPressed();
	void Client_HandleSprintPressed();
	UFUNCTION(Server, Reliable)
	void Server_HandleSprintPressed();
	UFUNCTION(NetMulticast, Reliable)
	void Remote_HandleSprintPressed();

	void HandleCrouchPressed();
	void Client_HandleCrouchPressed();
	UFUNCTION(Server, Reliable)
	void Server_HandleCrouchPressed();
	UFUNCTION(NetMulticast, Reliable)
	void Remote_HandleCrouchPressed();

	void HandleShoulderSwapPressed();
	void Client_HandleShoulderSwapPressed();
	UFUNCTION(Server, Reliable)
	void Server_HandleShoulderSwapPressed();
	UFUNCTION(NetMulticast, Reliable)
	void Remote_HandleShoulderSwapPressed();

	void HandleDivePressed();
	void Client_HandleDivePressed();
	UFUNCTION(Server, Reliable)
	void Server_HandleDivePressed();
	UFUNCTION(NetMulticast, Reliable)
	void Remote_HandleDivePressed();

	void HandleADSPressed();
	void Client_HandleADSPressed();
	UFUNCTION(Server, Reliable)
	void Server_HandleADSPressed();
	UFUNCTION(NetMulticast, Reliable)
	void Remote_HandleADSPressed();

	void HandleInteractPressed();
	void Client_HandleInteractPressed();
	UFUNCTION(Server, Reliable)
	void Server_HandleInteractPressed();

	void HandleDropPressed();
	void Client_HandleDropPressed();
	UFUNCTION(Server, Reliable)
	void Server_HandleDropPressed();

	void HandleFirePressed();
	void Client_HandleFirePressed();
	UFUNCTION(Server, Reliable)
	void Server_HandleFirePressed();
	UFUNCTION(NetMulticast, Reliable)
	void Remote_HandleFirePressed();

	void HandleFireReleased();
	void Client_HandleFireReleased();
	UFUNCTION(Server, Reliable)
	void Server_HandleFireReleased();
	UFUNCTION(NetMulticast, Reliable)
	void Remote_HandleFireReleased();
	UFUNCTION(Server, Reliable)
	void Server_BulletShot(const FVector StartPosition, const FVector EndPosition);
	UFUNCTION(NetMulticast, Reliable)
	void Remote_BulletShot(const FVector StartPosition, const FVector EndPosition);

#pragma endregion

	ATPPlayer(const class FObjectInitializer& PCIP);
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void ResetPreRecoilCamera();

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
	void HandleVaultAnimMoveForwardComplete();

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
