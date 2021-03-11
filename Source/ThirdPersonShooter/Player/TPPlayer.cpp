// Fill out your copyright notice in the Description page of Project Settings.


#include "./TPPlayer.h"
#include "../CustomCompoenents/Misc/InteractionComponent.h"
#include "../Weapons/BaseShootingWeapon.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#if WITH_EDITOR
#include "UnrealEd.h"
#endif

// #if WITH_EDITOR
// 	GUnrealEd->PlayWorld->bDebugPauseExecution = true;
// #endif

ATPPlayer::ATPPlayer()
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

	WeaponAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponAttachPoint"));
	WeaponAttachPoint->SetupAttachment(GetMesh());

	InteractCastPoint = CreateDefaultSubobject<USceneComponent>(TEXT("InteractCastPoint"));
	InteractCastPoint->SetupAttachment(FollowCamera);

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
	_cameraBoomLength = FVector2D(CameraDefaultBoomLength, CameraDefaultBoomLength);
	_shoulderCameraLerpAmount = 0;
	_runLerpAmount = 1;

	WeaponAttachPoint->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), RightHandSocket);
	WeaponAttachPoint->SetRelativeLocation(RightAttachmentLocation);
	WeaponAttachPoint->SetRelativeRotation(RightAttachmentRotation);
}

void ATPPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCapsuleSize(DeltaTime);
	UpdateFalling(DeltaTime);
	UpdateShoulderCamera(DeltaTime);
	UpdateDive(DeltaTime);
	UpdateRunMeshRotation(DeltaTime);
	UpdateVaultForward(DeltaTime);

	CheckAndActivateWallClimb();
}

void ATPPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ATPPlayer::HandleJumpPressed);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ATPPlayer::HandleJumpReleased);
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ATPPlayer::HandleSprintPressed);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ATPPlayer::HandleCrouchPressed);
	PlayerInputComponent->BindAction("ShoulderSwap", EInputEvent::IE_Pressed, this, &ATPPlayer::HandleShoulderSwapPressed);
	PlayerInputComponent->BindAction("Dive", EInputEvent::IE_Pressed, this, &ATPPlayer::HandleDivePressed);
	PlayerInputComponent->BindAction("ADS", EInputEvent::IE_Pressed, this, &ATPPlayer::HandleADSPressed);
	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &ATPPlayer::HandleInteractPressed);

	PlayerInputComponent->BindAxis("Turn", this, &ATPPlayer::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ATPPlayer::LookUpRate);
	PlayerInputComponent->BindAxis("MoveForward", this, &ATPPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPPlayer::MoveRight);
}

void ATPPlayer::MoveForward(const float Value)
{
	_verticalInput = Value;
	if (!CanAcceptPlayerInput())
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
	if (_isClimbing)
	{
		return;
	}

	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
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
	Jump();
}

void ATPPlayer::HandleJumpReleased()
{
	_isJumpPressed = false;
}

void ATPPlayer::UpdateFalling(const float DeltaTime)
{
	const bool isFalling = GetCharacterMovement()->IsFalling();

	if (!isFalling && _lastFrameFalling) // Handle Character Landed...
	{
		_lastFrameFalling = isFalling;
		if (_isAdsBeforeFalling)
		{
			HandleADSPressed();
		}
	}
	else if (isFalling && !_lastFrameFalling) // Handle Character Jumped...
	{
		if (_isInAds)
		{
			_isAdsBeforeFalling = true;
			HandleADSPressed();
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

	if (_isInAds)
	{
		HandleADSPressed();
	}

	_runStartRotation = GetMesh()->GetRelativeRotation();
	_runEndRotation = _runStartRotation;
	_runLerpAmount = 0;

	RemovePlayerMovementState(EPlayerMovementState::Crouch);
	PushPlayerMovementState(EPlayerMovementState::Run);
	ApplyChangesToCharacter();
}

void ATPPlayer::UpdateRunMeshRotation(const float DeltaTime)
{
	if (_runLerpAmount >= 1)
	{
		return;
	}

	_runLerpAmount += RunLerpSpeed * DeltaTime;
	const FRotator mappedRotation = FMath::Lerp(_runStartRotation, _runEndRotation, _runLerpAmount);
	GetMesh()->SetRelativeRotation(mappedRotation);

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
			_runStartRotation = GetMesh()->GetRelativeRotation();
			_runEndRotation = FRotator(0, DefaultMeshZPosition, 0);
			_runLerpAmount = 0;
		}
	}

	if (_runLerpAmount >= 1)
	{
		GetMesh()->SetRelativeRotation(_runEndRotation);
	}
}

void ATPPlayer::ResetPreRunRotation(const bool ForceReset)
{
	if (!ForceReset)
	{
		_runStartRotation = GetMesh()->GetRelativeRotation();
		_runEndRotation = FRotator(0, MeshDefaultZRotation, 0);
		_runLerpAmount = 0;
	}
	else
	{
		GetMesh()->SetRelativeRotation(FRotator(0, MeshDefaultZRotation, 0));
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

void ATPPlayer::HandleShoulderSwapPressed()
{
	_isLeftShoulder = !_isLeftShoulder;

	if (_isLeftShoulder)
	{
		const FDetachmentTransformRules detachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
		                                                                            EDetachmentRule::KeepWorld,
		                                                                            EDetachmentRule::KeepWorld,
		                                                                            true);
		WeaponAttachPoint->DetachFromComponent(detachmentRules);

		WeaponAttachPoint->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), LeftHandSocket);
		WeaponAttachPoint->SetRelativeLocation(LeftAttachmentLocation);
		WeaponAttachPoint->SetRelativeRotation(LeftAttachmentRotation);
	}
	else
	{
		const FDetachmentTransformRules detachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
		                                                                            EDetachmentRule::KeepWorld,
		                                                                            EDetachmentRule::KeepWorld,
		                                                                            true);
		WeaponAttachPoint->DetachFromComponent(detachmentRules);

		WeaponAttachPoint->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), RightHandSocket);
		WeaponAttachPoint->SetRelativeLocation(RightAttachmentLocation);
		WeaponAttachPoint->SetRelativeRotation(RightAttachmentRotation);
	}

	ShoulderSwapNotify();
}

void ATPPlayer::UpdateShoulderCamera(const float DeltaTime)
{
	_shoulderStartPosition = FollowCamera->GetRelativeLocation();
	_cameraBoomLength = FVector2D(0, CameraBoom->TargetArmLength);
	_shoulderCameraLerpAmount = 0;
	if (_isLeftShoulder)
	{
		if (_isInAds)
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
		if (_isInAds)
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

	if (_isInAds)
	{
		HandleADSPressed();
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
	_diveStartRotation = GetMesh()->GetRelativeRotation();
	_diveEndRotation = UKismetMathLibrary::InverseTransformRotation(GetActorTransform(), targetRotation);
	_diveLerpAmount = 0;
	_acceptDiveInput = false;
	bUseControllerRotationYaw = false;

	PushPlayerMovementState(EPlayerMovementState::Dive);
	ApplyChangesToCharacter();

	PlayerDiveNotify(HasPlayerState(EPlayerMovementState::Crouch));
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
		const FVector direction = UKismetMathLibrary::GetForwardVector(GetControlRotation()) * _verticalInput +
			UKismetMathLibrary::GetRightVector(GetControlRotation()) * _horizontalInput;
		AddMovementInput(direction, 1);

		SetActorRotation(FRotator(0, GetControlRotation().Yaw, 0));
	}

	if (_diveLerpAmount < 1)
	{
		const FRotator mappedRotation = FMath::Lerp(_diveStartRotation, _diveEndRotation, _diveLerpAmount);
		GetMesh()->SetRelativeRotation(mappedRotation);
		_diveLerpAmount += DiveLerpSpeed * DeltaTime;

		if (_diveLerpAmount >= 1)
		{
			GetMesh()->SetRelativeRotation(_diveEndRotation);
		}
	}
}

void ATPPlayer::HandleDiveAnimComplete()
{
	GetMesh()->SetRelativeRotation(FRotator(0, MeshDefaultZRotation, 0));
	bUseControllerRotationYaw = true;

	RemovePlayerMovementState(EPlayerMovementState::Dive);
	ApplyChangesToCharacter();

	if (GetTopPlayerState() == EPlayerMovementState::Run)
	{
		_runStartRotation = FRotator(0, MeshDefaultZRotation, 0);
		_runEndRotation = _runStartRotation;
		_runLerpAmount = 0;
	}
}

void ATPPlayer::HandleDiveResetAngle()
{
	_diveStartRotation = GetMesh()->GetRelativeRotation();
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

	_isInAds = !_isInAds;
}

bool ATPPlayer::CanAcceptADSInput() const
{
	if (_lastFrameFalling || GetTopPlayerState() == EPlayerMovementState::Dive || _isClimbing)
	{
		return false;
	}

	return true;
}

void ATPPlayer::HandleInteractPressed()
{
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);

	FVector startLocation = InteractCastPoint->GetComponentLocation();
	FVector endLocation = startLocation + FollowCamera->GetForwardVector() * InteractionDistance;

	DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::Red, false, 10);

	FHitResult hitResult;
	bool hit = GetWorld()->LineTraceSingleByChannel(hitResult, startLocation, endLocation, ECollisionChannel::ECC_Visibility, collisionParams);
	if (hit && hitResult.GetActor() != nullptr)
	{
		DrawDebugSphere(GetWorld(), hitResult.Location, 10, 16, FColor::Red, false, 10);

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

bool ATPPlayer::CanAcceptPlayerInput() const
{
	if (GetTopPlayerState() == EPlayerMovementState::Dive || _isClimbing)
	{
		return false;
	}

	return true;
}

void ATPPlayer::SetCapsuleData(float TargetHeight, float TargetRadius, float MeshTargetPosition)
{
	_capsuleHeight = FVector2D(TargetHeight, GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	_capsuleRadius = FVector2D(TargetRadius, GetCapsuleComponent()->GetUnscaledCapsuleRadius());
	_meshLocation = FVector2D(MeshTargetPosition, GetMesh()->GetRelativeLocation().Z);
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
	const float meshLocation = FMath::Lerp(_meshLocation.Y, _meshLocation.X, _capsuleLerpAmount);

	GetCapsuleComponent()->SetCapsuleHalfHeight(currentHeight);
	GetCapsuleComponent()->SetCapsuleRadius(currentRadius);

	const FVector meshRelativeLocation = GetMesh()->GetRelativeLocation();
	GetMesh()->SetRelativeLocation(FVector(meshRelativeLocation.X, meshRelativeLocation.Y, meshLocation));

	_capsuleLerpAmount += CapsuleSizeLerpRate * DeltaTime;
	if (_capsuleLerpAmount > 1)
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(_capsuleHeight.X);
		GetCapsuleComponent()->SetCapsuleRadius(_capsuleRadius.X);
		GetMesh()->SetRelativeLocation(FVector(meshRelativeLocation.X, meshRelativeLocation.Y, _meshLocation.X));
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
	FHitResult hitResult;

	const FVector startLocation = GetActorLocation();
	const FVector endLocation = GetActorForwardVector() * WallClimbForwardCheck + startLocation;

	const bool didCollide = GetWorld()->SweepSingleByChannel(hitResult, startLocation, endLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, collisionShape);
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
	FHitResult hitResult;

	const FVector startLocation = GetActorLocation() + WallClimbUpOffset + GetActorForwardVector() * WallClimbHeightForwardCheck;
	const FVector endLocation = startLocation - WallClimbUpDownOffset;

	const bool didCollide = GetWorld()->SweepSingleByChannel(hitResult, startLocation, endLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, collisionShape);
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
	FHitResult hitResult;

	const FVector startLocation = GetActorLocation() + WallClimbUpOffset + GetActorForwardVector() * (WallClimbHeightForwardCheck + VaultThicknessDistance);
	const FVector endLocation = startLocation - VaultDownOffset;

	const bool didCollide = GetWorld()->SweepSingleByChannel(hitResult, startLocation, endLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, collisionShape);
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
	const FVector socketLocation = GetMesh()->GetSocketLocation("pelvisSocket");
	const float difference = socketLocation.Z - hitLocation.Z;

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Climb Height: " + FString::SanitizeFloat(difference));

	if (!_isClimbing && difference >= -WallClimbHeight && difference <= -WallClimbMinHeight)
	{
		_preVJMovementState = GetTopPlayerState();
		ResetPreRunRotation();
		if (_isInAds)
		{
			HandleADSPressed();
		}

		GetCharacterMovement()->StopMovementImmediately();
		RemovePlayerMovementState(EPlayerMovementState::Run);
		RemovePlayerMovementState(EPlayerMovementState::Crouch);
		ApplyChangesToCharacter();

		const FRotator targetRotation = UKismetMathLibrary::MakeRotFromX(wallNormal * -1);
		const FVector newPosition = wallNormal * ClimbAnimXOffset + wallLocation;
		const FVector delta = FVector(newPosition.X, newPosition.Y, heightLocation.Z - ClimbAnimZOffset);

		_isClimbing = true;
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
	const FVector socketLocation = GetMesh()->GetSocketLocation("pelvisSocket");
	const float difference = socketLocation.Z - hitLocation.Z;

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Vault Height: " + FString::SanitizeFloat(difference));

	if (!_isClimbing && difference >= VaultWallMinHeight && difference <= VaultWallHeight)
	{
		_preVJMovementState = GetTopPlayerState();
		ResetPreRunRotation();
		if (_isInAds)
		{
			HandleADSPressed();
		}

		GetCharacterMovement()->StopMovementImmediately();
		RemovePlayerMovementState(EPlayerMovementState::Run);
		RemovePlayerMovementState(EPlayerMovementState::Crouch);
		ApplyChangesToCharacter();

		const FRotator targetRotation = UKismetMathLibrary::MakeRotFromX(wallNormal * -1);
		const FVector newPosition = wallNormal * VaultAnimXOffset + wallLocation;
		const FVector delta = FVector(newPosition.X, newPosition.Y, heightLocation.Z - VaultAnimZOffset);

		_isClimbing = true;
		PlayerVaultNotify(targetRotation, delta);

		return true;
	}

	return false;
}

bool ATPPlayer::CheckAndActivateWallClimb()
{
	if (_verticalInput == 1 && _isJumpPressed && !_isClimbing && CanAcceptPlayerInput())
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
	_isClimbing = false;

	if (_preVJMovementState == EPlayerMovementState::Run)
	{
		HandleSprintPressed();
	}
	else if (_preVJMovementState == EPlayerMovementState::Crouch)
	{
		HandleCrouchPressed();
	}
}

void ATPPlayer::HandleVaultAnimMoveForwardComplete()
{
	const FVector forwardVector = GetActorForwardVector();
	const FRotator rotation = UKismetMathLibrary::MakeRotFromX(forwardVector);
	SetActorRotation(FRotator(0, rotation.Yaw, 0));

	GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
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
	_isClimbing = false;
	_updateVaultForward = false;
	GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);

	if (_preVJMovementState == EPlayerMovementState::Run)
	{
		HandleSprintPressed();
	}
	else if (_preVJMovementState == EPlayerMovementState::Crouch)
	{
		HandleCrouchPressed();
	}
}

void ATPPlayer::UpdateVaultForward(const float DeltaTime)
{
	if (!_isClimbing || !_updateVaultForward)
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

void ATPPlayer::PickupWeapon(ABaseShootingWeapon* Weapon)
{
	const FAttachmentTransformRules attachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget,
	                                                                            EAttachmentRule::SnapToTarget,
	                                                                            EAttachmentRule::KeepWorld,
	                                                                            true);

	Weapon->PickupWeapon();
	Weapon->AttachToComponent(WeaponAttachPoint, attachmentRules);

	_currentWeapon = Weapon;
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
	else if (GetTopPlayerState() != EPlayerMovementState::Run)
	{
		return false;
	}

	return true;
}


bool ATPPlayer::IsInAds()
{
	return _isInAds;
}

bool ATPPlayer::IsLeftShoulder()
{
	return _isLeftShoulder;
}
