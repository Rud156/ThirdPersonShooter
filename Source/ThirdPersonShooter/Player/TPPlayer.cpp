// Fill out your copyright notice in the Description page of Project Settings.


#include "./TPPlayer.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ATPPlayer::ATPPlayer()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	PrimaryActorTick.bCanEverTick = true;
}

void ATPPlayer::BeginPlay()
{
	Super::BeginPlay();

	PushPlayerMovementState(EPlayerMovementState::Walk);
	ApplyChangesToCharacter();

	_isLeftShoulder = false;
	_shoulderStartPosition = CameraRightShoulder;
	_shoulderEndPosition = CameraRightShoulder;
	_meshStartRotation = MeshRightRotation;
	_meshEndRotation = MeshRightRotation;
	_shoulderCameraLerpAmount = 0;
}

void ATPPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCapsuleSize(DeltaTime);
	UpdateShoulderCamera(DeltaTime);
	UpdateDive(DeltaTime);
}

void ATPPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ATPPlayer::HandleJumpPressed);
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ATPPlayer::HandleSprintPressed);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ATPPlayer::HandleCrouchPressed);
	PlayerInputComponent->BindAction("ShoulderSwap", EInputEvent::IE_Pressed, this, &ATPPlayer::HandleShoulderSwapPressed);
	PlayerInputComponent->BindAction("Dive", EInputEvent::IE_Pressed, this, &ATPPlayer::HandleDivePressed);
	PlayerInputComponent->BindAction("ADS", EInputEvent::IE_Pressed, this, &ATPPlayer::HandleADSPressed);

	PlayerInputComponent->BindAxis("Turn", this, &ATPPlayer::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ATPPlayer::LookUpRate);
	PlayerInputComponent->BindAxis("MoveForward", this, &ATPPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPPlayer::MoveRight);
}

void ATPPlayer::MoveForward(const float Value)
{
	_verticalInput = Value;
	if (GetTopPlayerState() == EPlayerMovementState::Dive)
	{
		return;
	}

	if (Controller != nullptr && Value != 0.0f)
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
			RemovePlayerMovementState(EPlayerMovementState::Run);
			ApplyChangesToCharacter();
		}
	}
}

void ATPPlayer::MoveRight(const float Value)
{
	_horizontalInput = Value;
	if (GetTopPlayerState() == EPlayerMovementState::Dive)
	{
		return;
	}

	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator rotation = Controller->GetControlRotation();
		const FRotator yawRotation(0, rotation.Yaw, 0);

		const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(direction, Value);
	}
}

void ATPPlayer::LookUpRate(const float Value)
{
	AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATPPlayer::TurnAtRate(const float Value)
{
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATPPlayer::HandleJumpPressed()
{
	if (GetTopPlayerState() == EPlayerMovementState::Crouch)
	{
		RemovePlayerMovementState(EPlayerMovementState::Crouch);
		ApplyChangesToCharacter();
		return;
	}

	PlayerJumpNotify();
	Jump();
}

void ATPPlayer::HandleSprintPressed()
{
	RemovePlayerMovementState(EPlayerMovementState::Crouch);
	PushPlayerMovementState(EPlayerMovementState::Run);
	ApplyChangesToCharacter();
}

void ATPPlayer::HandleCrouchPressed()
{
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

void ATPPlayer::HandleShoulderSwapPressed()
{
	_isLeftShoulder = !_isLeftShoulder;
	_shoulderStartPosition = FollowCamera->GetRelativeLocation();
	_meshStartRotation = GetMesh()->GetRelativeRotation().Euler();
	_shoulderCameraLerpAmount = 0;

	if (_isLeftShoulder)
	{
		if (_isInAds)
		{
			_shoulderEndPosition = CameraADSLeftShoulder;
		}
		else
		{
			_shoulderEndPosition = CameraLeftShoulder;
		}

		_meshEndRotation = MeshLeftRotation;
	}
	else
	{
		if (_isInAds)
		{
			_shoulderEndPosition = CameraADSRightShoulder;
		}
		else
		{
			_shoulderEndPosition = CameraRightShoulder;
		}

		_meshEndRotation = MeshRightRotation;
	}
}

void ATPPlayer::UpdateShoulderCamera(const float DeltaTime)
{
	if (_shoulderCameraLerpAmount > 1 || _shoulderCameraLerpAmount < 0)
	{
		return;
	}

	const FVector mappedLocation = FMath::Lerp(_shoulderStartPosition, _shoulderEndPosition, _shoulderCameraLerpAmount);
	FollowCamera->SetRelativeLocation(mappedLocation);

	const FVector mappedMeshRotation = FMath::Lerp(_meshStartRotation, _meshEndRotation, _shoulderCameraLerpAmount);
	GetMesh()->SetRelativeRotation(FRotator::MakeFromEuler(mappedMeshRotation));

	_shoulderCameraLerpAmount += CameraLerpSpeed * DeltaTime;

	if (_shoulderCameraLerpAmount >= 1)
	{
		FollowCamera->SetRelativeLocation(_shoulderEndPosition);
		GetMesh()->SetRelativeRotation(FRotator::MakeFromEuler(_meshEndRotation));
	}
}

void ATPPlayer::HandleDivePressed()
{
	const FVector direction = FVector(-_horizontalInput, _verticalInput, 0);
	_diveDirection = direction;

	RemovePlayerMovementState(EPlayerMovementState::Crouch);
	RemovePlayerMovementState(EPlayerMovementState::Run);
	PushPlayerMovementState(EPlayerMovementState::Dive);
	ApplyChangesToCharacter();

	PlayerDiveNotify();
}

void ATPPlayer::UpdateDive(const float DeltaTime)
{
	if (GetTopPlayerState() == EPlayerMovementState::Dive)
	{
		AddMovementInput(_diveDirection, 1);
	}
}

void ATPPlayer::HandleDiveAnimComplete()
{
	RemovePlayerMovementState(EPlayerMovementState::Dive);
	ApplyChangesToCharacter();
}

void ATPPlayer::HandleADSPressed()
{
	_isInAds = !_isInAds;
	_shoulderStartPosition = FollowCamera->GetRelativeLocation();
	_meshStartRotation = GetMesh()->GetRelativeRotation().Euler();
	_shoulderCameraLerpAmount = 0;

	if (_isLeftShoulder)
	{
		if (_isInAds)
		{
			_shoulderEndPosition = CameraADSLeftShoulder;
		}
		else
		{
			_shoulderEndPosition = CameraLeftShoulder;
		}

		_meshEndRotation = MeshLeftRotation;
	}
	else
	{
		if (_isInAds)
		{
			_shoulderEndPosition = CameraADSRightShoulder;
		}
		else
		{
			_shoulderEndPosition = CameraRightShoulder;
		}

		_meshEndRotation = MeshRightRotation;
	}
}

void ATPPlayer::SetCapsuleData(float TargetHeight, float TargetRadius)
{
	_capsuleHeight = FVector2D(TargetHeight, GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	_capsuleRadius = FVector2D(TargetRadius, GetCapsuleComponent()->GetUnscaledCapsuleRadius());
	_capsuleLerpAmount = 0;
}

void ATPPlayer::UpdateCapsuleSize(float DeltaTime)
{
	if (_capsuleLerpAmount > 1 || _capsuleLerpAmount < 0)
	{
		return;
	}

	const float currentHeight = FMath::Lerp(_capsuleHeight.Y, _capsuleHeight.X, _capsuleLerpAmount);
	const float currentRadius = FMath::Lerp(_capsuleRadius.Y, _capsuleRadius.X, _capsuleLerpAmount);

	GetCapsuleComponent()->SetCapsuleHalfHeight(currentHeight);
	GetCapsuleComponent()->SetCapsuleRadius(currentRadius);

	_capsuleLerpAmount += CapsuleSizeLerpRate * DeltaTime;
	if (_capsuleLerpAmount > 1)
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(_capsuleHeight.X);
		GetCapsuleComponent()->SetCapsuleRadius(_capsuleRadius.X);
	}
}

void ATPPlayer::PushPlayerMovementState(EPlayerMovementState MovementState)
{
	_movementStack.Add(MovementState);
}

void ATPPlayer::RemovePlayerMovementState(const EPlayerMovementState MovementState)
{
	for (int i = _movementStack.Num() - 1; i >= 0; i--)
	{
		if (_movementStack[i] == MovementState)
		{
			_movementStack.RemoveAt(i);
		}
	}
}

bool ATPPlayer::HasPlayerState(const EPlayerMovementState MovementState)
{
	for (int i = 0; i < _movementStack.Num(); i++)
	{
		if (_movementStack[i] == MovementState)
		{
			return true;
		}
	}

	return false;
}

EPlayerMovementState ATPPlayer::GetTopPlayerState() const
{
	if (_movementStack.Num() <= 0)
	{
		return EPlayerMovementState::None;
	}

	return _movementStack.Last();
}

void ATPPlayer::ApplyChangesToCharacter()
{
	SetCapsuleData(DefaultHalfHeight, DefaultRadius);

	switch (GetTopPlayerState())
	{
	case EPlayerMovementState::None:
		break;

	case EPlayerMovementState::Walk:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		break;

	case EPlayerMovementState::Run:
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		break;

	case EPlayerMovementState::Crouch:
		GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
		SetCapsuleData(CrouchHalfHeight, CrouchRadius);
		break;

	case EPlayerMovementState::Dive:
		GetCharacterMovement()->MaxWalkSpeed = DiveSpeed;
		SetCapsuleData(CrouchHalfHeight, CrouchRadius);
		break;

	default:
		break;
	}
}

float ATPPlayer::GetVerticalInput() const
{
	return _verticalInput;
}

float ATPPlayer::GetHorizontalInput() const
{
	return _horizontalInput;
}