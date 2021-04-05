// Fill out your copyright notice in the Description page of Project Settings.

#include "./TPPlayer.h"
#include "../Utils/Structs.h"
#include "./CustomPlayerMovement.h"
#include "../CustomCompoenents/Health/HealthAndDamageComponent.h"
#include "../CustomCompoenents/Health/HealthDisplayComponent.h"
#include "../CustomCompoenents/Misc/InteractionComponent.h"
#include "../CustomCompoenents/Misc/DamageBulletDisplayComponent.h"
#include "../CustomCompoenents/Misc/DefaultWeaponComponent.h"
#include "../Weapons/BaseShootingWeapon.h"
#include "../Common/SpawnLocationsController.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// #if WITH_EDITOR
// #include "UnrealEd.h"
// #endif

// #if WITH_EDITOR
// 	GUnrealEd->PlayWorld->bDebugPauseExecution = true;
// #endif

ATPPlayer::ATPPlayer(const class FObjectInitializer& PCIP) : Super(PCIP.SetDefaultSubobjectClass<UCustomPlayerMovement>(ACharacter::CharacterMovementComponentName))
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetupAttachment(RootComponent);

	WeaponAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponAttachPoint"));
	WeaponAttachPoint->SetupAttachment(PlayerMesh);

	InteractCastPoint = CreateDefaultSubobject<USceneComponent>(TEXT("InteractCastPoint"));
	InteractCastPoint->SetupAttachment(FollowCamera);

	GunShootClearPoint = CreateDefaultSubobject<USceneComponent>(TEXT("GunShootClearPoint"));
	GunShootClearPoint->SetupAttachment(FollowCamera);

	PunchCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("PunchCollider"));
	PunchCollider->SetupAttachment(PlayerMesh);

	DamageBulletDisplay = CreateDefaultSubobject<UDamageBulletDisplayComponent>(TEXT("DamageBulletDisplay"));
	DamageBulletDisplay->SetupAttachment(RootComponent);

	HealthAndDamage = CreateDefaultSubobject<UHealthAndDamageComponent>(TEXT("HealthAndDamage"));
	DefaultWeaponComp = CreateDefaultSubobject<UDefaultWeaponComponent>(TEXT("DefaultWeaponComp"));
	HealthDisplayComp = CreateDefaultSubobject<UHealthDisplayComponent>(TEXT("HealthDisplayComp"));

	PrimaryActorTick.bCanEverTick = true;
}

void ATPPlayer::BeginPlay()
{
	Super::BeginPlay();

	HealthAndDamage->OnUnitDied.AddDynamic(this, &ATPPlayer::HandlePlayerDied);

	PushPlayerMovementState(EPlayerMovementState::Walk);
	ApplyChangesToCharacter();
	ClearRecoilData();

	N_IsCameraLeftShoulder = false;
	_shoulderStartPosition = CameraRightShoulder;
	_shoulderEndPosition = CameraRightShoulder;
	_cameraBoomLength = FVector2D(CameraDefaultBoomLength, CameraDefaultBoomLength);
	_shoulderCameraLerpAmount = 0;
	_runLerpAmount = 1;

	WeaponAttachPoint->AttachToComponent(PlayerMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), RightHandSocket);
	WeaponAttachPoint->SetRelativeLocation(RightAttachmentLocation);
	WeaponAttachPoint->SetRelativeRotation(RightAttachmentRotation);

	PunchCollider->AttachToComponent(PlayerMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,
	                                                                       EAttachmentRule::KeepWorld, true), RightHand);
	PunchCollider->SetRelativeLocation(RightHandPunchLocation);
	PunchCollider->SetRelativeRotation(RightHandPunchRotation);

	if (HasAuthority())
	{
		PunchCollider->OnComponentBeginOverlap.AddDynamic(this, &ATPPlayer::HandlePunchCollided);
	}
}

void ATPPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SendLookRotationToServer();
	SendPlayerInputsToServer();
	UpdateMovementServerRemote();

	UpdatePlayerDied(DeltaTime);

	UpdateCapsuleSize(DeltaTime);
	UpdateFalling(DeltaTime);
	UpdateShoulderCamera(DeltaTime);
	UpdateDive(DeltaTime);
	UpdateRunMeshRotation(DeltaTime);
	UpdateVaultForward(DeltaTime);
	UpdateRecoilCamera(DeltaTime);
	UpdateFirePressed(DeltaTime);

	CheckAndActivateWallClimb();
}

void ATPPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DropCurrentWeapon();
}

void ATPPlayer::MoveForward(const float Value)
{
	_verticalInput = Value;
	if (!CanAcceptPlayerInput())
	{
		return;
	}

	if (Value != 0.0f)
	{
		const FRotator rotation = GetControlRotation();
		const FRotator yawRotation(0, rotation.Yaw, 0);

		const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(direction, Value);
	}

	if (GetTopPlayerState() == EPlayerMovementState::Run)
	{
		if (Value != 1)
		{
			ResetPreRunRotation();
			RemovePlayerMovementState(EPlayerMovementState::Run);
			ApplyChangesToCharacter();
		}
	}
}

void ATPPlayer::MoveRight(const float Value)
{
	_horizontalInput = Value;
	if (!CanAcceptPlayerInput())
	{
		return;
	}

	if (Value != 0.0f)
	{
		const FRotator rotation = GetControlRotation();
		const FRotator yawRotation(0, rotation.Yaw, 0);

		const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(direction, Value);
	}
}

void ATPPlayer::UpdateMovementServerRemote()
{
	if (IsLocallyControlled())
	{
		return;
	}

	if (!CanAcceptPlayerInput())
	{
		return;
	}

	// Vertical
	if (_verticalInput != 0.0f)
	{
		const FRotator rotation = GetControlRotation();
		const FRotator yawRotation(0, rotation.Yaw, 0);

		const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(direction, _verticalInput);
	}
	if (GetTopPlayerState() == EPlayerMovementState::Run)
	{
		if (_verticalInput != 1)
		{
			ResetPreRunRotation();
			RemovePlayerMovementState(EPlayerMovementState::Run);
			ApplyChangesToCharacter();
		}
	}

	// Horizontal
	if (_horizontalInput != 0.0f)
	{
		const FRotator rotation = GetControlRotation();
		const FRotator yawRotation(0, rotation.Yaw, 0);

		const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(direction, _horizontalInput);
	}
}

void ATPPlayer::Client_MoveForward(const float Value)
{
	MoveForward(Value);
}

void ATPPlayer::Client_MoveRight(const float Value)
{
	MoveRight(Value);
}

void ATPPlayer::SendPlayerInputsToServer()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (!HasAuthority())
	{
		Server_ReceiveInput(_verticalInput, _horizontalInput);
	}
	else
	{
		Remote_ReceiveInput(_verticalInput, _horizontalInput);
	}
}

void ATPPlayer::Server_ReceiveInput_Implementation(const float Vertical, const float Horizontal)
{
	Remote_ReceiveInput(Vertical, Horizontal);
}

void ATPPlayer::Remote_ReceiveInput_Implementation(const float Vertical, const float Horizontal)
{
	if (IsLocallyControlled())
	{
		return;
	}

	_verticalInput = Vertical;
	_horizontalInput = Horizontal;
}

void ATPPlayer::LookUpRate(const float Value)
{
	if (IsLocallyControlled())
	{
		AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
		const FRotator controlRotation = GetControlRotation();
		SetCameraBoomPitchRotation(controlRotation);
	}
}

void ATPPlayer::SendLookRotationToServer()
{
	if (IsLocallyControlled())
	{
		const FRotator controlRotation = GetControlRotation();
		if (!HasAuthority())
		{
			Server_LookUpControlRotation(controlRotation);
		}
		else
		{
			Remote_LookUpControlRotation(controlRotation);
		}
	}
}

void ATPPlayer::SetCameraBoomPitchRotation(const FRotator ControlRotation) const
{
	FRotator worldRotation = CameraBoom->GetComponentRotation();
	worldRotation.Pitch = ControlRotation.Pitch;
	CameraBoom->SetWorldRotation(worldRotation);
}

void ATPPlayer::Client_LookUpRate(const float Value)
{
	if (IsPlayerDead())
	{
		return;
	}

	LookUpRate(Value);
}

void ATPPlayer::Server_LookUpControlRotation_Implementation(const FRotator ControlRotation)
{
	Remote_LookUpControlRotation(ControlRotation);
}

void ATPPlayer::Remote_LookUpControlRotation_Implementation(const FRotator ControlRotation)
{
	if (IsLocallyControlled())
	{
		return;
	}

	SetCameraBoomPitchRotation(ControlRotation);
}

void ATPPlayer::TurnAtRate(const float Value)
{
	if (N_IsClimbing)
	{
		return;
	}

	if (IsLocallyControlled() || HasAuthority())
	{
		AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void ATPPlayer::Client_TurnAtRate(const float Value)
{
	if (IsPlayerDead())
	{
		return;
	}

	TurnAtRate(Value);
	if (!HasAuthority())
	{
		Server_TurnAtRate(Value);
	}
	else
	{
		Remote_TurnAtRate(Value);
	}
}

void ATPPlayer::Server_TurnAtRate_Implementation(const float Value)
{
	TurnAtRate(Value);
	Remote_TurnAtRate(Value);
}

void ATPPlayer::Remote_TurnAtRate_Implementation(const float Value)
{
	if (IsLocallyControlled() || HasAuthority())
	{
		return;
	}

	TurnAtRate(Value);
}

void ATPPlayer::HandleJumpPressed()
{
	if (!CanAcceptPlayerInput())
	{
		return;
	}

	_isJumpPressed = true;
	const bool success = CheckAndActivateWallClimb();

	if (success)
	{
		return;
	}

	if (GetTopPlayerState() == EPlayerMovementState::Crouch)
	{
		RemovePlayerMovementState(EPlayerMovementState::Crouch);
		ApplyChangesToCharacter();
		return;
	}

	PlayerJumpNotify();

	if (IsLocallyControlled() || HasAuthority())
	{
		Jump();
	}
}

void ATPPlayer::Client_HandleJumpPressed()
{
	HandleJumpPressed();
	if (!HasAuthority())
	{
		Server_HandleJumpPressed();
	}
	else
	{
		Remote_HandleJumpPressed();
	}
}

void ATPPlayer::Server_HandleJumpPressed_Implementation()
{
	HandleJumpPressed();
	Remote_HandleJumpPressed();
}

void ATPPlayer::Remote_HandleJumpPressed_Implementation()
{
	if (IsLocallyControlled() || HasAuthority())
	{
		return;
	}

	HandleJumpPressed();
}

void ATPPlayer::HandleJumpReleased()
{
	_isJumpPressed = false;
}

void ATPPlayer::Client_HandleJumpReleased()
{
	HandleJumpReleased();
	if (!HasAuthority())
	{
		Server_HandleJumpReleased();
	}
	else
	{
		Remote_HandleJumpReleased();
	}
}

void ATPPlayer::Server_HandleJumpReleased_Implementation()
{
	HandleJumpReleased();
	Remote_HandleJumpReleased();
}

void ATPPlayer::Remote_HandleJumpReleased_Implementation()
{
	if (IsLocallyControlled() || HasAuthority())
	{
		return;
	}

	HandleJumpReleased();
}

void ATPPlayer::UpdateFalling(const float DeltaTime)
{
	const bool isFalling = GetCharacterMovement()->IsFalling();

	if (!isFalling && _lastFrameFalling) // Handle Character Landed...
	{
		_lastFrameFalling = isFalling;
		if (_isAdsBeforeFalling)
		{
			Client_HandleADSPressed();
		}
	}
	else if (isFalling && !_lastFrameFalling) // Handle Character Jumped...
	{
		if (N_IsCameraInAds)
		{
			_isAdsBeforeFalling = true;
			Client_HandleADSPressed();
		}
		else
		{
			_isAdsBeforeFalling = false;
		}
	}

	_lastFrameFalling = isFalling;
}

void ATPPlayer::HandleSprintPressed()
{
	if (!CanAcceptPlayerInput())
	{
		return;
	}

	if (N_IsCameraInAds)
	{
		Client_HandleADSPressed();
	}

	_runStartRotation = PlayerMesh->GetRelativeRotation();
	_runEndRotation = _runStartRotation;
	_runLerpAmount = 0;

	RemovePlayerMovementState(EPlayerMovementState::Crouch);
	PushPlayerMovementState(EPlayerMovementState::Run);
	ApplyChangesToCharacter();
}

void ATPPlayer::Client_HandleSprintPressed()
{
	HandleSprintPressed();
	if (!HasAuthority())
	{
		Server_HandleSprintPressed();
	}
	else
	{
		Remote_HandleSprintPressed();
	}
}

void ATPPlayer::Server_HandleSprintPressed_Implementation()
{
	HandleSprintPressed();
	Remote_HandleSprintPressed();
}

void ATPPlayer::Remote_HandleSprintPressed_Implementation()
{
	if (IsLocallyControlled() || HasAuthority())
	{
		return;
	}

	HandleSprintPressed();
}

void ATPPlayer::UpdateRunMeshRotation(const float DeltaTime)
{
	if (_runLerpAmount >= 1)
	{
		return;
	}

	_runLerpAmount += RunLerpSpeed * DeltaTime;
	const FRotator mappedRotation = FMath::Lerp(_runStartRotation, _runEndRotation, _runLerpAmount);
	PlayerMesh->SetRelativeRotation(mappedRotation);

	if (GetTopPlayerState() == EPlayerMovementState::Run)
	{
		const bool isInAir = GetCharacterMovement()->IsFalling();
		if (!isInAir)
		{
			const FVector direction = (GetActorForwardVector() * _verticalInput + GetActorRightVector() * _horizontalInput);
			const FRotator targetRotation = direction.Rotation() + FRotator(0, MeshDefaultZRotation, 0);

			_runStartRotation = mappedRotation;
			_runEndRotation = UKismetMathLibrary::InverseTransformRotation(GetActorTransform(), targetRotation);
			_runLerpAmount = 0;
		}
		else
		{
			_runStartRotation = PlayerMesh->GetRelativeRotation();
			_runEndRotation = FRotator(0, MeshDefaultZRotation, 0);
			_runLerpAmount = 0;
		}
	}

	// This will only happen if the player is not running to reset default rotation
	if (_runLerpAmount >= 1)
	{
		PlayerMesh->SetRelativeRotation(FRotator(0, MeshDefaultZRotation, 0));
	}
}

void ATPPlayer::ResetPreRunRotation(const bool ForceReset)
{
	if (!ForceReset)
	{
		_runStartRotation = PlayerMesh->GetRelativeRotation();
		_runEndRotation = FRotator(0, MeshDefaultZRotation, 0);
		_runLerpAmount = 0;
	}
	else
	{
		_runStartRotation = PlayerMesh->GetRelativeRotation();
		_runEndRotation = FRotator(0, MeshDefaultZRotation, 0);
		PlayerMesh->SetRelativeRotation(_runEndRotation);
	}
}

void ATPPlayer::HandleCrouchPressed()
{
	if (!CanAcceptPlayerInput())
	{
		return;
	}

	if (GetTopPlayerState() == EPlayerMovementState::Run)
	{
		ResetPreRunRotation();
		RemovePlayerMovementState(EPlayerMovementState::Run);
	}

	if (GetTopPlayerState() == EPlayerMovementState::Crouch)
	{
		RemovePlayerMovementState(EPlayerMovementState::Crouch);
	}
	else
	{
		PushPlayerMovementState(EPlayerMovementState::Crouch);
	}

	ApplyChangesToCharacter();
}

void ATPPlayer::Client_HandleCrouchPressed()
{
	HandleCrouchPressed();
	if (!HasAuthority())
	{
		Server_HandleCrouchPressed();
	}
	else
	{
		Remote_HandleCrouchPressed();
	}
}

void ATPPlayer::Server_HandleCrouchPressed_Implementation()
{
	HandleCrouchPressed();
	Remote_HandleCrouchPressed();
}

void ATPPlayer::Remote_HandleCrouchPressed_Implementation()
{
	if (IsLocallyControlled() || HasAuthority())
	{
		return;
	}

	HandleCrouchPressed();
}

void ATPPlayer::HandleShoulderSwapPressed()
{
	N_IsCameraLeftShoulder = !N_IsCameraLeftShoulder;

	if (N_IsCameraLeftShoulder)
	{
		const FDetachmentTransformRules detachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
		                                                                            EDetachmentRule::KeepWorld,
		                                                                            EDetachmentRule::KeepWorld,
		                                                                            true);

		WeaponAttachPoint->DetachFromComponent(detachmentRules);
		PunchCollider->DetachFromComponent(detachmentRules);

		WeaponAttachPoint->AttachToComponent(PlayerMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), LeftHandSocket);
		WeaponAttachPoint->SetRelativeLocation(LeftAttachmentLocation);
		WeaponAttachPoint->SetRelativeRotation(LeftAttachmentRotation);

		PunchCollider->AttachToComponent(PlayerMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,
		                                                                       EAttachmentRule::KeepWorld, true), LeftHand);
		PunchCollider->SetRelativeLocation(LeftHandPunchLocation);
		PunchCollider->SetRelativeRotation(LeftHandPunchRotation);
	}
	else
	{
		const FDetachmentTransformRules detachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
		                                                                            EDetachmentRule::KeepWorld,
		                                                                            EDetachmentRule::KeepWorld,
		                                                                            true);

		WeaponAttachPoint->DetachFromComponent(detachmentRules);
		PunchCollider->DetachFromComponent(detachmentRules);

		WeaponAttachPoint->AttachToComponent(PlayerMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), RightHandSocket);
		WeaponAttachPoint->SetRelativeLocation(RightAttachmentLocation);
		WeaponAttachPoint->SetRelativeRotation(RightAttachmentRotation);

		PunchCollider->AttachToComponent(PlayerMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,
		                                                                       EAttachmentRule::KeepWorld, true), RightHand);
		PunchCollider->SetRelativeLocation(RightHandPunchLocation);
		PunchCollider->SetRelativeRotation(RightHandPunchRotation);
	}

	ShoulderSwapNotify();
}

void ATPPlayer::Client_HandleShoulderSwapPressed()
{
	HandleShoulderSwapPressed();
	if (!HasAuthority())
	{
		Server_HandleShoulderSwapPressed();
	}
	else
	{
		Remote_HandleShoulderSwapPressed();
	}
}

void ATPPlayer::Server_HandleShoulderSwapPressed_Implementation()
{
	HandleShoulderSwapPressed();
	Remote_HandleShoulderSwapPressed();
}

void ATPPlayer::Remote_HandleShoulderSwapPressed_Implementation()
{
	if (IsLocallyControlled() || HasAuthority())
	{
		return;
	}

	HandleShoulderSwapPressed();
}

void ATPPlayer::UpdateShoulderCamera(const float DeltaTime)
{
	_shoulderStartPosition = FollowCamera->GetRelativeLocation();
	_cameraBoomLength = FVector2D(0, CameraBoom->TargetArmLength);
	_shoulderCameraLerpAmount = 0;
	if (N_IsCameraLeftShoulder)
	{
		if (N_IsCameraInAds)
		{
			_shoulderEndPosition = CameraADSLeftShoulder;
			_cameraBoomLength.X = CameraADSBoomLength;
		}
		else
		{
			_shoulderEndPosition = GetTopPlayerState() == EPlayerMovementState::Run ? RunCameraLeftPosition : CameraLeftShoulder;
			_cameraBoomLength.X = CameraDefaultBoomLength;
		}
	}
	else
	{
		if (N_IsCameraInAds)
		{
			_shoulderEndPosition = CameraADSRightShoulder;
			_cameraBoomLength.X = CameraADSBoomLength;
		}
		else
		{
			_shoulderEndPosition = GetTopPlayerState() == EPlayerMovementState::Run ? RunCameraRightPosition : CameraRightShoulder;
			_cameraBoomLength.X = CameraDefaultBoomLength;
		}
	}

	_shoulderCameraLerpAmount += CameraLerpSpeed * DeltaTime;

	const FVector mappedLocation = FMath::Lerp(_shoulderStartPosition, _shoulderEndPosition, _shoulderCameraLerpAmount);
	FollowCamera->SetRelativeLocation(mappedLocation);

	const float mappedBoomLength = FMath::Lerp(_cameraBoomLength.Y, _cameraBoomLength.X, _shoulderCameraLerpAmount);
	CameraBoom->TargetArmLength = mappedBoomLength;
}

void ATPPlayer::HandleDivePressed()
{
	if (!CanAcceptPlayerInput())
	{
		return;
	}

	if (GetTopPlayerState() == EPlayerMovementState::Run)
	{
		ResetPreRunRotation(true);
	}

	if (N_IsCameraInAds)
	{
		if (IsLocallyControlled())
		{
			Client_HandleADSPressed();
		}
	}

	FVector direction;
	if (_horizontalInput == 0 && _verticalInput == 0)
	{
		direction = GetActorForwardVector();
	}
	else
	{
		direction = GetActorForwardVector() * _verticalInput + GetActorRightVector() * _horizontalInput;
	}

	const FRotator targetRotation = direction.Rotation() + FRotator(0, MeshDefaultZRotation, 0);

	_diveDirection = direction;
	_diveStartRotation = PlayerMesh->GetRelativeRotation();
	_diveEndRotation = UKismetMathLibrary::InverseTransformRotation(GetActorTransform(), targetRotation);
	_diveLerpAmount = 0;
	_acceptDiveInput = false;
	bUseControllerRotationYaw = false;

	if (N_CurrentWeapon != nullptr)
	{
		N_CurrentWeapon->HideWeapon();
	}

	ResetPlayerAnimationData();
	PushPlayerMovementState(EPlayerMovementState::Dive);
	ApplyChangesToCharacter();

	PlayerDiveNotify(HasPlayerState(EPlayerMovementState::Crouch));
}

void ATPPlayer::Client_HandleDivePressed()
{
	HandleDivePressed();
	if (!HasAuthority())
	{
		Server_HandleDivePressed();
	}
	else
	{
		Remote_HandleDivePressed();
	}
}

void ATPPlayer::Server_HandleDivePressed_Implementation()
{
	HandleDivePressed();
	Remote_HandleDivePressed();
}

void ATPPlayer::Remote_HandleDivePressed_Implementation()
{
	if (IsLocallyControlled() || HasAuthority())
	{
		return;
	}

	HandleDivePressed();
}

void ATPPlayer::UpdateDive(const float DeltaTime)
{
	if (GetTopPlayerState() != EPlayerMovementState::Dive)
	{
		return;
	}

	if (!_acceptDiveInput)
	{
		AddMovementInput(_diveDirection, 1);
	}
	else
	{
		FRotator controlRotation;
		if (IsLocallyControlled())
		{
			controlRotation = GetControlRotation();
		}
		else
		{
			controlRotation = CameraBoom->GetComponentRotation();
		}

		const FVector direction = UKismetMathLibrary::GetForwardVector(controlRotation) * _verticalInput +
			UKismetMathLibrary::GetRightVector(controlRotation) * _horizontalInput;

		AddMovementInput(direction, 1);
		SetActorRotation(FRotator(0, controlRotation.Yaw, 0));
	}

	if (_diveLerpAmount < 1)
	{
		const FRotator mappedRotation = FMath::Lerp(_diveStartRotation, _diveEndRotation, _diveLerpAmount);
		PlayerMesh->SetRelativeRotation(mappedRotation);

		_diveLerpAmount += DiveLerpSpeed * DeltaTime;
		if (_diveLerpAmount >= 1)
		{
			PlayerMesh->SetRelativeRotation(_diveEndRotation);
		}
	}
}

void ATPPlayer::HandleDiveAnimComplete()
{
	PlayerMesh->SetRelativeRotation(FRotator(0, MeshDefaultZRotation, 0));
	bUseControllerRotationYaw = true;

	RemovePlayerMovementState(EPlayerMovementState::Dive);
	ApplyChangesToCharacter();

	if (GetTopPlayerState() == EPlayerMovementState::Run)
	{
		_runStartRotation = FRotator(0, MeshDefaultZRotation, 0);
		_runEndRotation = _runStartRotation;
		_runLerpAmount = 0;
	}

	if (N_CurrentWeapon != nullptr)
	{
		N_CurrentWeapon->ShowWeapon();
	}
}

void ATPPlayer::HandleDiveResetAngle()
{
	_diveStartRotation = PlayerMesh->GetRelativeRotation();
	_diveEndRotation = FRotator(0, MeshDefaultZRotation, 0);
	_diveLerpAmount = 0;
	_acceptDiveInput = true;

	GetCharacterMovement()->MaxWalkSpeed = DiveGetUpSpeed;
}

void ATPPlayer::HandleADSPressed()
{
	if (!CanAcceptADSInput())
	{
		return;
	}

	if (GetTopPlayerState() == EPlayerMovementState::Run)
	{
		ResetPreRunRotation();
		RemovePlayerMovementState(EPlayerMovementState::Run);
		ApplyChangesToCharacter();
	}

	N_IsCameraInAds = !N_IsCameraInAds;
}

void ATPPlayer::Client_HandleADSPressed()
{
	HandleADSPressed();
	if (!HasAuthority())
	{
		Server_HandleADSPressed();
	}
	else
	{
		Remote_HandleADSPressed();
	}
}

void ATPPlayer::Server_HandleADSPressed_Implementation()
{
	HandleADSPressed();
	Remote_HandleADSPressed();
}

void ATPPlayer::Remote_HandleADSPressed_Implementation()
{
	if (IsLocallyControlled() || HasAuthority())
	{
		return;
	}

	HandleADSPressed();
}

bool ATPPlayer::CanAcceptADSInput() const
{
	if (N_IsPlayerDead || _lastFrameFalling || GetTopPlayerState() == EPlayerMovementState::Dive || N_IsClimbing)
	{
		return false;
	}

	return true;
}

void ATPPlayer::HandleInteractPressed()
{
	if (!HasAuthority())
	{
		return;
	}

	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);

	FVector startLocation = InteractCastPoint->GetComponentLocation();
	FVector endLocation = startLocation + FollowCamera->GetForwardVector() * InteractionDistance;

	FHitResult hitResult;
	bool hit = GetWorld()->LineTraceSingleByChannel(hitResult, startLocation, endLocation, ECollisionChannel::ECC_Visibility, collisionParams);
	if (hit && hitResult.GetActor() != nullptr)
	{
		auto actor = hitResult.GetActor();
		auto actorComponent = actor->GetComponentByClass(UInteractionComponent::StaticClass());
		auto interactionComponent = Cast<UInteractionComponent>(actorComponent);

		if (interactionComponent != nullptr && interactionComponent->CanInteract())
		{
			ABaseShootingWeapon* weapon = Cast<ABaseShootingWeapon>(actor);
			if (weapon != nullptr)
			{
				PickupWeapon(weapon);
			}
		}
	}
}

void ATPPlayer::Client_HandleInteractPressed()
{
	Server_HandleInteractPressed();
}

void ATPPlayer::Server_HandleInteractPressed_Implementation()
{
	HandleInteractPressed();
}

void ATPPlayer::HandleDropPressed()
{
	if (!HasAuthority())
	{
		return;
	}

	if (N_CurrentWeapon != nullptr)
	{
		DropWeapon(N_CurrentWeapon);
	}
}

void ATPPlayer::Client_HandleDropPressed()
{
	Server_HandleDropPressed();
}

void ATPPlayer::Server_HandleDropPressed_Implementation()
{
	HandleDropPressed();
}

bool ATPPlayer::CanAcceptPlayerInput() const
{
	if (N_IsPlayerDead || GetTopPlayerState() == EPlayerMovementState::Dive || N_IsClimbing)
	{
		return false;
	}

	return true;
}

void ATPPlayer::SetCapsuleData(float TargetHeight, float TargetRadius, float MeshTargetPosition)
{
	_capsuleHeight = FVector2D(TargetHeight, GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	_capsuleRadius = FVector2D(TargetRadius, GetCapsuleComponent()->GetUnscaledCapsuleRadius());
	_meshLocation = FVector2D(MeshTargetPosition, PlayerMesh->GetRelativeLocation().Z);
	_capsuleLerpAmount = 0;
}

void ATPPlayer::UpdateCapsuleSize(const float DeltaTime)
{
	if (_capsuleLerpAmount > 1 || _capsuleLerpAmount < 0)
	{
		return;
	}

	const float currentHeight = FMath::Lerp(_capsuleHeight.Y, _capsuleHeight.X, _capsuleLerpAmount);
	const float currentRadius = FMath::Lerp(_capsuleRadius.Y, _capsuleRadius.X, _capsuleLerpAmount);
	const float meshLocation = FMath::Lerp(_meshLocation.Y, _meshLocation.X, _capsuleLerpAmount);
	const FVector meshRelativeLocation = PlayerMesh->GetRelativeLocation();

	GetCapsuleComponent()->SetCapsuleHalfHeight(currentHeight);
	GetCapsuleComponent()->SetCapsuleRadius(currentRadius);
	PlayerMesh->SetRelativeLocation(FVector(meshRelativeLocation.X, meshRelativeLocation.Y, meshLocation));

	_capsuleLerpAmount += CapsuleSizeLerpRate * DeltaTime;
	if (_capsuleLerpAmount > 1)
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(_capsuleHeight.X);
		GetCapsuleComponent()->SetCapsuleRadius(_capsuleRadius.X);
		PlayerMesh->SetRelativeLocation(FVector(meshRelativeLocation.X, meshRelativeLocation.Y, _meshLocation.X));
	}
}

void ATPPlayer::PushPlayerMovementState(EPlayerMovementState MovementState)
{
	N_MovementStack.Add(MovementState);
}

void ATPPlayer::RemovePlayerMovementState(const EPlayerMovementState MovementState)
{
	for (int i = N_MovementStack.Num() - 1; i >= 0; i--)
	{
		if (N_MovementStack[i] == MovementState)
		{
			N_MovementStack.RemoveAt(i);
		}
	}
}

bool ATPPlayer::HasPlayerState(const EPlayerMovementState MovementState)
{
	for (int i = 0; i < N_MovementStack.Num(); i++)
	{
		if (N_MovementStack[i] == MovementState)
		{
			return true;
		}
	}

	return false;
}

bool ATPPlayer::IsMoving() const
{
	const EPlayerMovementState topState = GetTopPlayerState();
	switch (topState)
	{
	case EPlayerMovementState::None:
		return false;

	case EPlayerMovementState::Walk:
		{
			if (GetVelocity().Size() == 0)
			{
				return false;
			}

			return true;
		}

	case EPlayerMovementState::Run:
	case EPlayerMovementState::Crouch:
	case EPlayerMovementState::Dive:
		return true;

	default:
		return false;
	}
}

EPlayerMovementState ATPPlayer::GetTopPlayerState() const
{
	if (N_MovementStack.Num() <= 0)
	{
		return EPlayerMovementState::None;
	}

	return N_MovementStack.Last();
}

void ATPPlayer::ApplyChangesToCharacter()
{
	switch (GetTopPlayerState())
	{
	case EPlayerMovementState::None:
		SetCapsuleData(DefaultHalfHeight, DefaultRadius, DefaultMeshZPosition);
		break;

	case EPlayerMovementState::Walk:
		SetCapsuleData(DefaultHalfHeight, DefaultRadius, DefaultMeshZPosition);
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		break;

	case EPlayerMovementState::Run:
		SetCapsuleData(DefaultHalfHeight, DefaultRadius, DefaultMeshZPosition);
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		break;

	case EPlayerMovementState::Crouch:
		SetCapsuleData(CrouchHalfHeight, CrouchRadius, CrouchMeshZPosition);
		GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
		break;

	case EPlayerMovementState::Dive:
		GetCharacterMovement()->MaxWalkSpeed = DiveSpeed;
		break;

	default:
		SetCapsuleData(DefaultHalfHeight, DefaultRadius, DefaultMeshZPosition);
		break;
	}
}

void ATPPlayer::WallClimbForwardTrace(bool& CanClimb, bool& CanVault)
{
	const FCollisionShape collisionShape = FCollisionShape::MakeSphere(10);
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);

	FHitResult hitResult;

	const FVector startLocation = GetActorLocation();
	const FVector endLocation = GetActorForwardVector() * WallClimbForwardCheck + startLocation;

	const bool didCollide = GetWorld()->SweepSingleByChannel(hitResult, startLocation, endLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, collisionShape,
	                                                         collisionParams);
	if (didCollide)
	{
		bool hasClimbTag = hitResult.GetComponent() != nullptr && hitResult.GetComponent()->ComponentHasTag(WallClimbableTag);
		if (!hasClimbTag)
		{
			hasClimbTag = hitResult.GetActor() != nullptr && hitResult.GetActor()->ActorHasTag(WallClimbableTag);
		}

		bool hasVaultTag = hitResult.GetComponent() != nullptr && hitResult.GetComponent()->ComponentHasTag(VaultTag);
		if (!hasVaultTag)
		{
			hasVaultTag = hitResult.GetActor() != nullptr && hitResult.GetActor()->ActorHasTag(VaultTag);
		}

		CanClimb = hasClimbTag;
		CanVault = hasVaultTag;
		_forwardTrace = hitResult;
	}
}

void ATPPlayer::WallClimbHeightTrace(bool& CanClimb, bool& CanVault)
{
	const FCollisionShape collisionShape = FCollisionShape::MakeSphere(10);
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);

	FHitResult hitResult;

	const FVector startLocation = GetActorLocation() + WallClimbUpOffset + GetActorForwardVector() * WallClimbHeightForwardCheck;
	const FVector endLocation = startLocation - WallClimbUpDownOffset;

	const bool didCollide = GetWorld()->SweepSingleByChannel(hitResult, startLocation, endLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, collisionShape,
	                                                         collisionParams);
	if (didCollide)
	{
		bool hasClimbTag = hitResult.GetComponent() != nullptr && hitResult.GetComponent()->ComponentHasTag(WallClimbableTag);
		if (!hasClimbTag)
		{
			hasClimbTag = hitResult.GetActor() != nullptr && hitResult.GetActor()->ActorHasTag(WallClimbableTag);
		}

		bool hasVaultTag = hitResult.GetComponent() != nullptr && hitResult.GetComponent()->ComponentHasTag(VaultTag);
		if (!hasVaultTag)
		{
			hasVaultTag = hitResult.GetActor() != nullptr && hitResult.GetActor()->ActorHasTag(VaultTag);
		}

		CanClimb = hasClimbTag;
		CanVault = hasVaultTag;
		_heightTrace = hitResult;
	}
}

bool ATPPlayer::VaultForwardHeightTrace()
{
	const FCollisionShape collisionShape = FCollisionShape::MakeSphere(10);
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);

	FHitResult hitResult;

	const FVector startLocation = GetActorLocation() + WallClimbUpOffset + GetActorForwardVector() * (WallClimbHeightForwardCheck + VaultThicknessDistance);
	const FVector endLocation = startLocation - VaultDownOffset;

	const bool didCollide = GetWorld()->SweepSingleByChannel(hitResult, startLocation, endLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, collisionShape,
	                                                         collisionParams);
	if (didCollide)
	{
		_forwardHeightTrace = hitResult;
		return true;
	}

	return false;
}

bool ATPPlayer::HandleWallClimb()
{
	const FVector wallNormal = _forwardTrace.Normal;
	const FVector wallLocation = _forwardTrace.Location;

	const FVector heightLocation = _heightTrace.Location;

	const FVector hitLocation = _heightTrace.Location;
	const FVector socketLocation = PlayerMesh->GetSocketLocation("pelvisSocket");
	const float difference = socketLocation.Z - hitLocation.Z;

	// GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Climb Height: " + FString::SanitizeFloat(difference));

	if (!N_IsClimbing && difference >= -WallClimbHeight && difference <= -WallClimbMinHeight)
	{
		_preVJMovementState = GetTopPlayerState();
		ResetPreRunRotation();
		if (N_IsCameraInAds)
		{
			Client_HandleADSPressed();
		}

		GetCharacterMovement()->StopMovementImmediately();
		RemovePlayerMovementState(EPlayerMovementState::Run);
		RemovePlayerMovementState(EPlayerMovementState::Crouch);
		ApplyChangesToCharacter();

		const FRotator targetRotation = UKismetMathLibrary::MakeRotFromX(wallNormal * -1);
		const FVector newPosition = wallNormal * ClimbAnimXOffset + wallLocation;
		const FVector delta = FVector(newPosition.X, newPosition.Y, heightLocation.Z - ClimbAnimZOffset);

		N_IsClimbing = true;
		_climbVaultAnimCompleteCalled = false;

		if (N_CurrentWeapon != nullptr)
		{
			N_CurrentWeapon->HideWeapon();
		}
		ResetPlayerAnimationData();
		PlayerClimbNotify(targetRotation, delta);

		return true;
	}

	return false;
}

bool ATPPlayer::HandleVault()
{
	const FVector wallNormal = _forwardTrace.Normal;
	const FVector wallLocation = _forwardTrace.Location;

	const FVector heightLocation = _heightTrace.Location;

	const FVector hitLocation = _heightTrace.Location;
	const FVector socketLocation = PlayerMesh->GetSocketLocation("pelvisSocket");
	const float difference = socketLocation.Z - hitLocation.Z;

	// GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Vault Height: " + FString::SanitizeFloat(difference));

	if (!N_IsClimbing && difference >= VaultWallMinHeight && difference <= VaultWallHeight)
	{
		_preVJMovementState = GetTopPlayerState();
		ResetPreRunRotation();
		if (N_IsCameraInAds)
		{
			Client_HandleADSPressed();
		}

		GetCharacterMovement()->StopMovementImmediately();
		RemovePlayerMovementState(EPlayerMovementState::Run);
		RemovePlayerMovementState(EPlayerMovementState::Crouch);
		ApplyChangesToCharacter();

		const FRotator targetRotation = UKismetMathLibrary::MakeRotFromX(wallNormal * -1);
		const FVector newPosition = wallNormal * VaultAnimXOffset + wallLocation;
		const FVector delta = FVector(newPosition.X, newPosition.Y, heightLocation.Z - VaultAnimZOffset);

		N_IsClimbing = true;
		_climbVaultAnimCompleteCalled = false;

		if (N_CurrentWeapon != nullptr)
		{
			N_CurrentWeapon->HideWeapon();
		}

		ResetPlayerAnimationData();
		PlayerVaultNotify(targetRotation, delta);

		return true;
	}

	return false;
}

bool ATPPlayer::CheckAndActivateWallClimb()
{
	if (_verticalInput == 1 && _isJumpPressed && !N_IsClimbing && CanAcceptPlayerInput())
	{
		bool forwardClimb = false;
		bool forwardVault = false;
		WallClimbForwardTrace(forwardClimb, forwardVault);

		bool heightClimb = false;
		bool heightVault = false;
		WallClimbHeightTrace(heightClimb, heightVault);

		const bool forwardHeightTrace = VaultForwardHeightTrace();
		if (forwardVault && heightVault && forwardHeightTrace && !_lastFrameFalling)
		{
			const bool vaulted = HandleVault();
			if (!vaulted && forwardClimb && heightClimb)
			{
				return HandleWallClimb();
			}

			return vaulted;
		}
		else if (forwardClimb && heightClimb)
		{
			return HandleWallClimb();
		}
	}

	return false;
}

void ATPPlayer::HandleClimbAnimComplete()
{
	if (_climbVaultAnimCompleteCalled)
	{
		return;
	}
	_climbVaultAnimCompleteCalled = true;
	N_IsClimbing = false;

	if (N_CurrentWeapon != nullptr)
	{
		N_CurrentWeapon->ShowWeapon();
	}

	if (_preVJMovementState == EPlayerMovementState::Run)
	{
		Client_HandleSprintPressed();
	}
	else if (_preVJMovementState == EPlayerMovementState::Crouch)
	{
		Client_HandleCrouchPressed();
	}
}

void ATPPlayer::HandleVaultAnimMoveForwardComplete()
{
	const FVector forwardVector = GetActorForwardVector();
	const FRotator rotation = UKismetMathLibrary::MakeRotFromX(forwardVector);
	SetActorRotation(FRotator(0, rotation.Yaw, 0));
	PlayerMesh->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	_updateVaultForward = true;

	float vaultDownDistance = _forwardHeightTrace.Location.Z - _heightTrace.Location.Z;
	if (vaultDownDistance < -VaultMoveDownMaxOffset)
	{
		vaultDownDistance += GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + VaultDownZDiffOffset;
		_vaultEndOffset = FVector2D(GetActorLocation().Z + vaultDownDistance, GetActorLocation().Z);
	}
	else
	{
		vaultDownDistance = _forwardHeightTrace.Location.Z + GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		_vaultEndOffset = FVector2D(vaultDownDistance, GetActorLocation().Z);
	}
	_vaultLerpAmount = 0;
}

void ATPPlayer::HandleVaultAnimComplete()
{
	if (_climbVaultAnimCompleteCalled)
	{
		return;
	}
	_climbVaultAnimCompleteCalled = true;
	N_IsClimbing = false;
	_updateVaultForward = false;
	PlayerMesh->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);

	if (N_CurrentWeapon != nullptr)
	{
		N_CurrentWeapon->ShowWeapon();
	}

	if (_preVJMovementState == EPlayerMovementState::Run)
	{
		Client_HandleSprintPressed();
	}
	else if (_preVJMovementState == EPlayerMovementState::Crouch)
	{
		Client_HandleCrouchPressed();
	}
}

void ATPPlayer::UpdateVaultForward(const float DeltaTime)
{
	if (!N_IsClimbing || !_updateVaultForward)
	{
		return;
	}

	const FVector direction = GetActorForwardVector() * _verticalInput + GetActorRightVector() * _horizontalInput;
	AddMovementInput(direction, 1);

	const FVector location = GetActorLocation();
	if (_vaultLerpAmount <= 1)
	{
		const float mappedZLocation = FMath::Lerp(_vaultEndOffset.Y, _vaultEndOffset.X, _vaultLerpAmount);
		_vaultLerpAmount += VaultLerpSpeed * DeltaTime;

		SetActorLocation(FVector(location.X, location.Y, mappedZLocation));
	}
	else if (_vaultLerpAmount > 1)
	{
		SetActorLocation(FVector(location.X, location.Y, _vaultEndOffset.X));
	}
}

void ATPPlayer::HandleFirePressed()
{
	if (N_CurrentWeapon != nullptr)
	{
		_firePressed = true;
	}
	else
	{
		if (_isPunching)
		{
			return;
		}

		_isPunching = true;
		_punchAlreadyHit = false;
		PlayerPunchNotify();
	}
}

void ATPPlayer::Client_HandleFirePressed()
{
	HandleFirePressed();
	if (!HasAuthority())
	{
		Server_HandleFirePressed();
	}
	else
	{
		Remote_HandleFirePressed();
	}
}

void ATPPlayer::Server_HandleFirePressed_Implementation()
{
	HandleFirePressed();
	Remote_HandleFirePressed();
}

void ATPPlayer::Remote_HandleFirePressed_Implementation()
{
	if (IsLocallyControlled() || HasAuthority())
	{
		return;
	}

	HandleFirePressed();
}

void ATPPlayer::HandleFireReleased()
{
	_firePressed = false;
}

void ATPPlayer::Client_HandleFireReleased()
{
	HandleFireReleased();
	if (!HasAuthority())
	{
		Server_HandleFireReleased();
	}
	else
	{
		Remote_HandleFireReleased();
	}
}

void ATPPlayer::Server_HandleFireReleased_Implementation()
{
	HandleFireReleased();
	Remote_HandleFireReleased();
}

void ATPPlayer::Remote_HandleFireReleased_Implementation()
{
	if (IsLocallyControlled() || HasAuthority())
	{
		return;
	}

	HandleFireReleased();
}

void ATPPlayer::UpdateRecoilCamera(const float DeltaTime)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (_recoilLerpAmount >= 1)
	{
		return;
	}

	if (_resetRecoil)
	{
		_recoilLerpSpeed = N_CurrentWeapon->RecoilResetLerpSpeed->GetFloatValue(_recoilLerpAmount);
	}

	const float currentAmount = _recoilLerpCurve->GetFloatValue(_recoilLerpAmount);
	const FVector2D currentInputAmount = FMath::Lerp(_startRecoilOffset, _targetRecoilOffset, currentAmount);
	_recoilLerpAmount += _recoilLerpSpeed * DeltaTime;
	_recoilLerpAmount = FMath::Clamp(_recoilLerpAmount, 0.0f, 1.0f);
	const float nextAmount = _recoilLerpCurve->GetFloatValue(_recoilLerpAmount);
	const FVector2D nextInputAmount = FMath::Lerp(_startRecoilOffset, _targetRecoilOffset, nextAmount);

	const FVector2D diff = nextInputAmount - currentInputAmount;
	AddControllerPitchInput(diff.Y * RecoilCameraMultiplier);
	AddControllerYawInput(diff.X * RecoilCameraMultiplier);

	if (_recoilLerpAmount >= 1)
	{
		_startRecoilOffset = _targetRecoilOffset;

		if (_resetRecoil)
		{
			ClearRecoilData();
			N_CurrentWeapon->ResetRecoilData(0);
		}
	}
}

void ATPPlayer::UpdateFirePressed(const float DeltaTime)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (!CanAcceptShootingInput() || N_CurrentWeapon == nullptr)
	{
		return;
	}

	if (_firePressed)
	{
		const int shootCount = N_CurrentWeapon->GetShootCountAndSaveRemainder();
		if (shootCount <= 0)
		{
			return;
		}

		const float currentAmount = _recoilLerpCurve->GetFloatValue(_recoilLerpAmount);
		const FVector2D currentRecoilAmount = FMath::Lerp(_startRecoilOffset, _targetRecoilOffset, currentAmount);
		if (_resetRecoil)
		{
			const float recoilLerp = 1 - currentAmount;
			const int bulletCount = static_cast<int>(N_CurrentWeapon->GetCurrentBulletCount() * recoilLerp);
			N_CurrentWeapon->ResetRecoilData(bulletCount);

			_targetRecoilOffset = currentRecoilAmount;
		}
		_startRecoilOffset = currentRecoilAmount;

		for (int i = 0; i < shootCount; i++)
		{
			const FRecoilOffset recoilOffset = N_CurrentWeapon->ShootWithRecoil(IsMoving(), N_IsCameraInAds);

			_targetRecoilOffset += recoilOffset.CrossHairOffset;
			_recoilLerpAmount = 0;
			_recoilLerpSpeed = N_CurrentWeapon->RecoilShootLerpSpeed;
			_resetRecoil = false;

			// This is the final raycast that will get hit...
			const FVector startPosition = InteractCastPoint->GetComponentLocation();
			const FVector endPosition = startPosition + FollowCamera->GetForwardVector() * MaxShootDistance +
				FollowCamera->GetUpVector() * recoilOffset.RayCastOffset.Y +
				FollowCamera->GetRightVector() * recoilOffset.RayCastOffset.X;

			BulletShot(startPosition, endPosition);
			if (!HasAuthority())
			{
				Server_BulletShot(startPosition, endPosition);
			}
			else
			{
				Remote_BulletShot(startPosition, endPosition);
			}
		}
		N_CurrentWeapon->ShootingSetComplete();
		N_CurrentWeapon->PlayAudio();
	}
}

void ATPPlayer::ResetPreRecoilCamera()
{
	const float amount = _recoilLerpCurve->GetFloatValue(_recoilLerpAmount);
	const FVector2D currentRecoilAmount = FMath::Lerp(_startRecoilOffset, _targetRecoilOffset, amount);
	_startRecoilOffset = currentRecoilAmount;
	_targetRecoilOffset = FVector2D::ZeroVector;
	_recoilLerpAmount = 0;
	_resetRecoil = true;
	_recoilLerpSpeed = N_CurrentWeapon->RecoilResetLerpSpeed->GetFloatValue(0);
}

void ATPPlayer::ClearRecoilData()
{
	_startRecoilOffset = FVector2D::ZeroVector;
	_targetRecoilOffset = FVector2D::ZeroVector;
	_recoilLerpAmount = 1;
	_recoilLerpSpeed = 1;
	_resetRecoil = false;
}

void ATPPlayer::Server_BulletShot_Implementation(const FVector StartPosition, const FVector EndPosition)
{
	if (!CanAcceptShootingInput() || N_CurrentWeapon == nullptr || !_firePressed)
	{
		return;
	}

	BulletShot(StartPosition, EndPosition);
	Remote_BulletShot(StartPosition, EndPosition);
}

void ATPPlayer::Remote_BulletShot_Implementation(const FVector StartPosition, const FVector EndPosition)
{
	if (IsLocallyControlled() || HasAuthority())
	{
		return;
	}

	BulletShot(StartPosition, EndPosition);
}

void ATPPlayer::BulletShot(const FVector StartPosition, const FVector EndPosition) const
{
	FVector sphereLocation = EndPosition;

	// Cast Initial Check
	FCollisionQueryParams wallCheckCollisionParams;
	wallCheckCollisionParams.AddIgnoredActor(this);

	FVector wallCheckStartPosition = N_CurrentWeapon->GetMesh()->GetSocketLocation(GunMuzzleSocketName);
	FVector wallCheckEndPosition = GunShootClearPoint->GetComponentLocation();
	FHitResult wallCheckHitResult;
	bool wallCheckHit = GetWorld()->LineTraceSingleByChannel(wallCheckHitResult, wallCheckStartPosition, wallCheckEndPosition, ECollisionChannel::ECC_Visibility,
	                                                         wallCheckCollisionParams);

	if (wallCheckHit)
	{
		DrawDebugLine(GetWorld(), wallCheckStartPosition, wallCheckEndPosition, FColor::Red, false, 1);
		sphereLocation = wallCheckHitResult.Location;
		if (HasAuthority())
		{
			CheckAndDealDamage(wallCheckHitResult.GetActor(), wallCheckHitResult.BoneName.ToString());
		}
	}
	else // If the First Line Trace does not hit it means LOS is clear. Shoot Normally
	{
		DrawDebugLine(GetWorld(), StartPosition, EndPosition, FColor::Red, false, 1);

		FCollisionQueryParams collisionParams;
		collisionParams.AddIgnoredActor(this);

		FHitResult hitResult;
		bool hit = GetWorld()->LineTraceSingleByChannel(hitResult, StartPosition, EndPosition, ECollisionChannel::ECC_Visibility, collisionParams);

		if (hit)
		{
			sphereLocation = hitResult.Location;
			if (HasAuthority())
			{
				CheckAndDealDamage(hitResult.GetActor(), hitResult.BoneName.ToString());
			}
		}
	}

	DrawDebugSphere(GetWorld(), sphereLocation, 5, 16, FColor::Red, false, 1);
}

void ATPPlayer::CheckAndDealDamage(AActor* HitActor, const FString BoneName) const
{
	if (!HasAuthority())
	{
		return;
	}

	if (HitActor != nullptr)
	{
		UActorComponent* healthActorComp = HitActor->GetComponentByClass(UHealthAndDamageComponent::StaticClass());
		if (healthActorComp != nullptr)
		{
			UHealthAndDamageComponent* healthAndDamageComponent = Cast<UHealthAndDamageComponent>(healthActorComp);

			const int damageAmount = N_CurrentWeapon->GetDamageForBone(BoneName);
			healthAndDamageComponent->TakeDamage(damageAmount);
		}
	}
}

void ATPPlayer::PickupWeapon(ABaseShootingWeapon* Weapon)
{
	const FAttachmentTransformRules attachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget,
	                                                                            EAttachmentRule::SnapToTarget,
	                                                                            EAttachmentRule::KeepWorld,
	                                                                            true);

	const bool pickupSuccess = Weapon->PickupWeapon(this);
	if (!pickupSuccess)
	{
		return;
	}

	Weapon->RecoilResetCallback.AddDynamic(this, &ATPPlayer::ResetPreRecoilCamera);
	Weapon->AttachToComponent(WeaponAttachPoint, attachmentRules);

	N_CurrentWeapon = Weapon;

	_recoilLerpCurve = N_CurrentWeapon->RecoilLerpCurve;
	ClearRecoilData();
}

void ATPPlayer::DropWeapon(ABaseShootingWeapon* Weapon)
{
	ClearRecoilData();

	if (Weapon == nullptr)
	{
		return;
	}

	const bool dropSuccess = Weapon->DropWeapon(this);
	if (!dropSuccess)
	{
		return;
	}

	const FDetachmentTransformRules detachRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
	                                                                        EDetachmentRule::KeepWorld,
	                                                                        EDetachmentRule::KeepWorld,
	                                                                        true);
	Weapon->DetachFromActor(detachRules);

	N_CurrentWeapon = nullptr;
}

void ATPPlayer::DropCurrentWeapon()
{
	if (N_CurrentWeapon != nullptr)
	{
		DropWeapon(N_CurrentWeapon);
	}
}

ABaseShootingWeapon* ATPPlayer::GetCurrentWeapon() const
{
	return N_CurrentWeapon;
}

void ATPPlayer::HandlePunchCollided(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                    const FHitResult& SweepResult)
{
	if (!_isPunching || _punchAlreadyHit || !HasAuthority() || OtherActor == this)
	{
		return;
	}

	UActorComponent* healthAndDamageComponent = OtherActor->GetComponentByClass(UHealthAndDamageComponent::StaticClass());
	UHealthAndDamageComponent* healthAndDamage = Cast<UHealthAndDamageComponent>(healthAndDamageComponent);
	if (healthAndDamage != nullptr)
	{
		healthAndDamage->TakeDamage(PunchDamageAmount);
		_punchAlreadyHit = true;
	}
}

void ATPPlayer::HandlePunchAnimComplete()
{
	_isPunching = false;
}

bool ATPPlayer::IsPunching()
{
	return _isPunching;
}

void ATPPlayer::HandlePlayerDied(AActor* Unit)
{
	if (HasAuthority())
	{
		N_IsPlayerDead = true;
		_respawnTimer = RespawnTimer;
		HidePlayer();
	}
}

void ATPPlayer::UpdatePlayerDied(const float DeltaTime)
{
	if (!HasAuthority())
	{
		return;
	}

	if (_respawnTimer > 0)
	{
		_respawnTimer -= DeltaTime;
		if (_respawnTimer <= 0)
		{
			N_IsPlayerDead = false;
			HealthAndDamage->SetHealth(HealthAndDamage->N_MaxHealth);

			AActor* spawnControllerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnLocationsController::StaticClass());
			ASpawnLocationsController* spawnController = Cast<ASpawnLocationsController>(spawnControllerActor);

			AActor* spawnLocationActor = spawnController->GetValidSpawnPointPlayer(this);
			const FVector spawnLocation = spawnLocationActor->GetActorLocation();
			SetActorLocation(spawnLocation, false, nullptr, ETeleportType::TeleportPhysics);

			ShowPlayer();
		}
	}
}

void ATPPlayer::HidePlayer() const
{
	GetCapsuleComponent()->SetCollisionProfileName(PlayerDeadCollisionName, true);
	GetMesh()->SetCollisionProfileName(PlayerMeshDeadCollisionName, true);
	PlayerMesh->SetHiddenInGame(true);
	PlayerMesh->SetCollisionProfileName(PlayerMeshDeadCollisionName, true);
	DamageBulletDisplay->SetTintColorAndOpacity(FLinearColor(1, 1, 1, 0));

	if (N_CurrentWeapon != nullptr)
	{
		N_CurrentWeapon->HideWeapon();
	}
}

void ATPPlayer::ShowPlayer() const
{
	GetCapsuleComponent()->SetCollisionProfileName(PlayerDefaultCollisionName, true);
	GetMesh()->SetCollisionProfileName(PlayerMeshDefaultCollisionName, true);
	PlayerMesh->SetHiddenInGame(false);
	PlayerMesh->SetCollisionProfileName(PlayerMeshDefaultCollisionName, true);
	DamageBulletDisplay->SetTintColorAndOpacity(FLinearColor(1, 1, 1, 1));

	if (N_CurrentWeapon != nullptr)
	{
		N_CurrentWeapon->ShowWeapon();
	}
}

void ATPPlayer::ResetPlayerAnimationData()
{
	_isPunching = false;
	_punchAlreadyHit = false;
}

bool ATPPlayer::CanAcceptShootingInput() const
{
	if (N_IsPlayerDead || !CanAcceptPlayerInput() || GetTopPlayerState() == EPlayerMovementState::Run)
	{
		return false;
	}

	return true;
}

float ATPPlayer::GetVerticalInput() const
{
	return _verticalInput;
}

float ATPPlayer::GetHorizontalInput() const
{
	return _horizontalInput;
}

bool ATPPlayer::IsRunning()
{
	if (_horizontalInput == 0 && _verticalInput == 0)
	{
		return false;
	}

	if (GetTopPlayerState() != EPlayerMovementState::Run)
	{
		return false;
	}

	return true;
}

bool ATPPlayer::IsInAds()
{
	return N_IsCameraInAds;
}

bool ATPPlayer::IsLeftShoulder()
{
	return N_IsCameraLeftShoulder;
}

bool ATPPlayer::IsPlayerDead()
{
	return N_IsPlayerDead;
}

FVector ATPPlayer::GetWeaponAttachPoint()
{
	if (N_CurrentWeapon == nullptr)
	{
		return FVector::ZeroVector;
	}
	else
	{
		USkeletalMeshComponent* weaponMesh = N_CurrentWeapon->GetMesh();
		if (N_IsCameraLeftShoulder)
		{
			return weaponMesh->GetSocketLocation("RightHandAttachPoint");
		}
		else
		{
			return weaponMesh->GetSocketLocation("LeftHandAttachPoint");
		}
	}
}

bool ATPPlayer::HasWeapon()
{
	return N_CurrentWeapon != nullptr;
}

void ATPPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPPlayer, N_MovementStack);
	DOREPLIFETIME(ATPPlayer, N_IsCameraLeftShoulder);
	DOREPLIFETIME(ATPPlayer, N_IsCameraInAds);
	DOREPLIFETIME(ATPPlayer, N_IsClimbing);
	DOREPLIFETIME(ATPPlayer, N_IsPlayerDead);
	DOREPLIFETIME(ATPPlayer, N_CurrentWeapon);
}

void ATPPlayer::OnDeathDataFromNetwork()
{
	if (N_IsPlayerDead)
	{
		HidePlayer();
	}
	else
	{
		ShowPlayer();
	}
}

void ATPPlayer::OnDataFromNetwork()
{
	ApplyChangesToCharacter();
}

void ATPPlayer::OnWeaponDataFromNetwork(ABaseShootingWeapon* PreviousWeapon)
{
	if (N_CurrentWeapon != PreviousWeapon)
	{
		DropWeapon(PreviousWeapon);
		if (N_CurrentWeapon != nullptr)
		{
			PickupWeapon(N_CurrentWeapon);
		}
	}
}
