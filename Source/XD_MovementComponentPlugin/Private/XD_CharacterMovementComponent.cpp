// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_CharacterMovementComponent.h"
#include "XD_MovementComponentFunctionLibrary.h"
#include <GameFramework/Character.h>
#include <Kismet/KismetMathLibrary.h>
#include <UnrealNetwork.h>


UXD_CharacterMovementComponent::UXD_CharacterMovementComponent()
	:bAutoUpdateLookingRotation(true),
	bCanSprint(true)
{
	SetIsReplicated(true);

	MovementState.bCanCrouch = true;
	MovementState.bCanSwim = true;
	MovementState.bCanFly = true;

	RotationRate = FRotator::ZeroRotator;
	bCanWalkOffLedgesWhenCrouching = true;
	CrouchedHalfHeight = 60.f;
}

void UXD_CharacterMovementComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UXD_CharacterMovementComponent, LookingRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UXD_CharacterMovementComponent, MovementInput, COND_SkipOwner);
}

void UXD_CharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	SetGait(ECharacterGait::Walking);
}

void UXD_CharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	CustomMovingTick(DeltaTime);

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UXD_CharacterMovementComponent::CustomMovingTick(float DeltaTime)
{
	if (GetOwner()->HasAuthority() || GetCharacterOwner()->IsLocallyControlled())
	{
		if (bAutoUpdateLookingRotation)
		{
			LookingRotation = GetCharacterOwing()->GetControlRotation();
		}

		MovementInput = UXD_MovementComponentFunctionLibrary::GetMovementInput(GetCharacterOwing());
	}
	AimYawDelta = UKismetMathLibrary::NormalizedDeltaRotator(LookingRotation, GetCharacterRotation()).Yaw;

	if (HasMovementInput())
	{
		MovementInputVelocityDifference = UKismetMathLibrary::NormalizedDeltaRotator(GetLastMovementInputRotation(), GetLastVelocityRotation()).Yaw;
	}
	else
	{
		MovementInputVelocityDifference = 0.f;
	}

	switch (ALS_MovementMode)
	{
	case EALS_MovementMode::Grounded:
		if (StanceState == ECharacterStanceState::Standing)
		{
			switch (CurrentGait)
			{
			case ECharacterGait::Running:
			case ECharacterGait::Sprinting:
				MaxAcceleration = RunningAcceleration * UKismetMathLibrary::MapRangeClamped(FMath::Abs(MovementInputVelocityDifference), 45.f, 130.f, 1.f, 0.2f);
				GroundFriction = RunningGroundFriction * UKismetMathLibrary::MapRangeClamped(FMath::Abs(MovementInputVelocityDifference), 45.f, 130.f, 1.f, 0.4f);
				break;
			}
		}
		break;
	case EALS_MovementMode::Ragdoll:
		//TODO Do While Ragdolling
		break;
	}

	if (GetCharacterOwing()->HasAuthority() || GetCharacterOwing()->IsLocallyControlled())
	{
		if (bInvokeSprint)
		{
			if (CanSprint())
			{
				SetGait(ECharacterGait::Sprinting);
			}
			else
			{
				SetGait(ECharacterGait::Running);
			}
		}

		switch (ALS_MovementMode)
		{
		case EALS_MovementMode::Grounded:
			if (!GetCharacterOwing()->IsPlayingRootMotion())
			{
				if (IsMoving())
				{
					switch (RotationMode)
					{
					case ECharacterRotationMode::VelocityDirection:
					{
						FRotator CharacterRotation = GetCharacterRotation();
						TargetRotation = FRotator(CharacterRotation.Pitch, GetLastVelocityRotation().Yaw, CharacterRotation.Roll);
						float InterpSpeed = CalculateRotationRate(165.f, 5.f, 375.f, 10.f);
						SetCharacterRotation(FMath::RInterpTo(CharacterRotation, TargetRotation, DeltaTime, InterpSpeed));
						break;
					}
					case ECharacterRotationMode::LookingDirection:
					{
						FRotator CharacterRotation = GetCharacterRotation();
						float InterpSpeed = bAiming ? CalculateRotationRate(165.f, 15.f, 375.f, 15.f) : CalculateRotationRate(165.f, 10.f, 375.f, 15.f);
						TargetRotation = FRotator(CharacterRotation.Pitch, LookingDirectionWithOffsetYaw(DeltaTime, 5.f, 60.f, -60.f, 120.f, -120.f, 5.f), CharacterRotation.Roll);
						SetCharacterRotation(FMath::RInterpTo(CharacterRotation, TargetRotation, DeltaTime, InterpSpeed));
						break;
					}
					}
				}
				else if (bAiming && RotationMode == ECharacterRotationMode::LookingDirection)
				{
					float AimYawLimit = 90.f;
					float InterpSpeed = 15.f;
					if (FMath::Abs(AimYawDelta) > AimYawLimit)
					{
						FRotator CharacterRotation = GetCharacterRotation();
						float Yaw = AimYawDelta > 0.f ? LookingRotation.Yaw - AimYawLimit : LookingRotation.Yaw + AimYawLimit;
						TargetRotation = FRotator(CharacterRotation.Pitch, Yaw, CharacterRotation.Roll);
						SetCharacterRotation(FMath::RInterpTo(CharacterRotation, TargetRotation, DeltaTime, InterpSpeed));
					}
				}
			}
			else
			{
				RotationRateMultiplier = 0.f;
				if (RootMotionRotationSpeed > 0.f && HasMovementInput())
				{
					FRotator CharacterRotation = GetCharacterRotation();
					TargetRotation = FRotator(CharacterRotation.Pitch, MovementInput.Rotation().Yaw, CharacterRotation.Roll);
					SetCharacterRotation(FMath::RInterpTo(CharacterRotation, TargetRotation, DeltaTime, RootMotionRotationSpeed));
				}
			}
			break;

		case EALS_MovementMode::Falling:

			break;
		}


		if (RotationRateMultiplier < 1.f)
		{
			RotationRateMultiplier += DeltaTime;
			if (RotationRateMultiplier > 1.f)
			{
				RotationRateMultiplier = 1.f;
			}
		}
	}
}

void UXD_CharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	switch (MovementMode)
	{
	case EMovementMode::MOVE_Walking:
	case EMovementMode::MOVE_NavWalking:
		SetALS_MovementMode(EALS_MovementMode::Grounded);
		break;
	case EMovementMode::MOVE_Falling:
		SetALS_MovementMode(EALS_MovementMode::Falling);
		break;
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UXD_CharacterMovementComponent::UpdateMovementSetting()
{
	switch (StanceState)
	{
	case ECharacterStanceState::Standing:
		MaxWalkSpeed = ChooseMaxWalkSpeed();
		break;
	case ECharacterStanceState::Crouching:
		MaxWalkSpeedCrouched = ChooseMaxWalkCrouchedSpeed();
		break;
	}
	
	MaxAcceleration = ChooseMaxAcceleration();
	BrakingDecelerationWalking = ChooseBrakingDeceleration();
	GroundFriction = ChooseGroundFriction();
}

float UXD_CharacterMovementComponent::ChooseMaxWalkSpeed() const
{
	switch (CurrentGait)
	{
	case ECharacterGait::Walking:
		return WalkingSpeed;
	case ECharacterGait::Running:
		return RunningSpeed;
	case ECharacterGait::Sprinting:
		return SprintingSpeed;
	}
	return 0.f;
}

float UXD_CharacterMovementComponent::ChooseMaxWalkCrouchedSpeed() const
{
	switch (CurrentGait)
	{
	case ECharacterGait::Walking:
	case ECharacterGait::Running:
		return CrouchingSpeed;
	case ECharacterGait::Sprinting:
		return CrouchingSprintingSpeed;
	}
	return 0.f;
}

float UXD_CharacterMovementComponent::ChooseMaxAcceleration() const
{
	switch (CurrentGait)
	{
	case ECharacterGait::Walking:
		return WalkingAcceleration;
	case ECharacterGait::Running:
	case ECharacterGait::Sprinting:
		return RunningAcceleration;
	}
	return 0.f;
}

float UXD_CharacterMovementComponent::ChooseBrakingDeceleration() const
{
	switch (CurrentGait)
	{
	case ECharacterGait::Walking:
		return WalkingDeceleration;
	case ECharacterGait::Running:
	case ECharacterGait::Sprinting:
		return RunningDeceleration;
	}
	return 0.f;
}

float UXD_CharacterMovementComponent::ChooseGroundFriction() const
{
	switch (CurrentGait)
	{
	case ECharacterGait::Walking:
		return WalkingGroundFriction;
	case ECharacterGait::Running:
	case ECharacterGait::Sprinting:
		return RunningGroundFriction;
	}
	return 0.f;
}

float UXD_CharacterMovementComponent::LookingDirectionWithOffsetYaw(float DeltaTime, float OffsetInterpSpeed, float NE, float NW, float SE, float SW, float Buffer)
{
	auto CardinalDirectionAngles = [this](float Value, float Min, float Max, float Buffer, ECardinalDirection CardinalDirection)
	{
		if (CardinalDirection == this->CardinalDirection)
		{
			return Value >= Min + Buffer && Value <= Max - Buffer;
		}
		else
		{
			return Value >= Min - Buffer && Value <= Max + Buffer;
		}
	};

	float Value = UKismetMathLibrary::NormalizedDeltaRotator(HasMovementInput() ? GetLastMovementInputRotation() : GetLastVelocityRotation(), LookingRotation).Yaw;

	if (CardinalDirectionAngles(Value, NW, NE, Buffer, ECardinalDirection::North))
	{
		CardinalDirection = ECardinalDirection::North;
	}
	else if (CardinalDirectionAngles(Value, NE, SE, Buffer, ECardinalDirection::East))
	{
		CardinalDirection = ECardinalDirection::East;
	}
	else if (CardinalDirectionAngles(Value, SW, NW, Buffer, ECardinalDirection::West))
	{
		CardinalDirection = ECardinalDirection::West;
	}
	else
	{
		CardinalDirection = ECardinalDirection::South;
	}

	float Target = Value;
	switch (CardinalDirection)
	{
	case ECardinalDirection::East:
		Target -= 90.f;
		break;
	case ECardinalDirection::West:
		Target += 90.f;
		break;
	case ECardinalDirection::South:
		if (Value > 0.f)
		{
			Target -= 180.f;
		}
		else
		{
			Target += 180.f;
		}
		break;
	}

	RotationOffset = FMath::FInterpTo(RotationOffset, Target, DeltaTime, OffsetInterpSpeed);

	return RotationOffset + LookingRotation.Yaw;
}

float UXD_CharacterMovementComponent::CalculateRotationRate(float SlowSpeed, float SlowSpeedRate, float FastSpeed, float FastSpeedRate) const
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;
	float HorizontalSpeed = Velocity.Size();
	if (HorizontalSpeed > SlowSpeed)
	{
		return FMath::Clamp(UKismetMathLibrary::MapRangeUnclamped(HorizontalSpeed, SlowSpeed, FastSpeed, SlowSpeedRate, FastSpeedRate) * RotationRateMultiplier, 0.1f, 15.f);
	}
	else
	{
		return FMath::Clamp(UKismetMathLibrary::MapRangeUnclamped(HorizontalSpeed, 0.f, SlowSpeed, 0.f, SlowSpeedRate) * RotationRateMultiplier, 0.1f, 15.f);
	}
}

FVector UXD_CharacterMovementComponent::GetVelocity() const
{
	switch (ALS_MovementMode)
	{
	case EALS_MovementMode::None:
	case EALS_MovementMode::Grounded:
	case EALS_MovementMode::Falling:
		return GetCharacterOwing()->GetVelocity();
	case EALS_MovementMode::Ragdoll:
		return GetCharacterOwing()->GetMesh()->GetComponentVelocity();
	}
	return FVector::ZeroVector;
}

bool UXD_CharacterMovementComponent::CanSprint() const
{
	if (bCanSprint)
	{
		switch (ALS_MovementMode)
		{
		case EALS_MovementMode::Grounded:
		case EALS_MovementMode::Falling:
			switch (RotationMode)
			{
			case ECharacterRotationMode::VelocityDirection:
				return true;
			case ECharacterRotationMode::LookingDirection:
				return FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(GetLastMovementInputRotation(), LookingRotation).Yaw) < 50.f;
			}
		}
	}
	return false;
}

bool UXD_CharacterMovementComponent::HasMovementInput() const
{
	return !GetMovementInput().Equals(FVector::ZeroVector, 0.0001f);
}

FRotator UXD_CharacterMovementComponent::GetLastVelocityRotation() const
{
	if (GetVelocity().Size() > 0.f)
	{
		return GetVelocity().Rotation();
	}
	else
	{
		return GetCharacterRotation();
	}
}

FRotator UXD_CharacterMovementComponent::GetLastMovementInputRotation() const
{
	if (GetMovementInput().Size() > 0.f)
	{
		return GetMovementInput().Rotation();
	}
	else
	{
		return GetCharacterRotation();
	}
}

bool UXD_CharacterMovementComponent::IsMoving() const
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return !Velocity.Equals(FVector::ZeroVector, 1.f);
}

void UXD_CharacterMovementComponent::SetALS_MovementMode(EALS_MovementMode NewMovementMode)
{
	if (NewMovementMode != ALS_MovementMode)
	{
		EALS_MovementMode PreALS_MovementMode = ALS_MovementMode;
		ALS_MovementMode = NewMovementMode;

		UpdateMovementSetting();

		OnALS_MovementModeChanged.Broadcast(PreALS_MovementMode, ALS_MovementMode);

// 		if (Character->IsLocallyControlled())
// 		{
// 			switch (ALS_PrevMovementMode)
// 			{
// 			case EALS_MovementMode::Grounded:
// 				JumpRotation = IsMoving() ? GetLastVelocityRotation() : GetCharacterRotation();
// 				RotationOffset = 0.f;
// 				break;
// 			case EALS_MovementMode::Ragdoll:
// 				JumpRotation = GetCharacterRotation();
// 				break;
// 			}
// 
// 			switch (ALS_MovementMode)
// 			{
// 			case EALS_MovementMode::Ragdoll:
// 				break;
// 			}
// 		}
	}
}

void UXD_CharacterMovementComponent::SetGait(ECharacterGait Value)
{
	if (Value != CurrentGait)
	{
		ECharacterGait PreInvokeGait = CurrentGait;
		CurrentGait = Value;
		UpdateMovementSetting();
		OnGaitChanged.Broadcast(PreInvokeGait, CurrentGait);
	}
}

void UXD_CharacterMovementComponent::SetStanceState(ECharacterStanceState Value)
{
	if (Value != StanceState)
	{
		ECharacterStanceState PreStanceState = StanceState;
		StanceState = Value;
		UpdateMovementSetting();

		OnStanceStateChanged.Broadcast(PreStanceState, StanceState);
	}
}

void UXD_CharacterMovementComponent::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);
	SetStanceState(ECharacterStanceState::Crouching);
}

void UXD_CharacterMovementComponent::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);
	SetStanceState(ECharacterStanceState::Standing);
}

void UXD_CharacterMovementComponent::SetRotationMode(ECharacterRotationMode Value)
{
	if (Value != RotationMode)
	{
		ECharacterRotationMode PreRotationMode = RotationMode;
		RotationMode = Value;

		if (IsMoving())
		{
			RotationRateMultiplier = 0.f;
		}

		OnRotationModeChanged.Broadcast(PreRotationMode, RotationMode);
	}
}

bool UXD_CharacterMovementComponent::IsSprinting() const
{
	return bCanSprint && bInvokeSprint && !GetCharacterOwner()->IsPlayingRootMotion() && (MovementMode == EMovementMode::MOVE_Walking || MovementMode == EMovementMode::MOVE_NavWalking) && GetVelocity().Size() > RunningSpeed && FMath::Abs((GetVelocity().Rotation() - GetCharacterRotation()).GetNormalized().Yaw) < 10.f;
}

FVector UXD_CharacterMovementComponent::GetMovementInput() const
{
	return MovementInput;
}

FRotator UXD_CharacterMovementComponent::GetCharacterRotation() const
{
	return GetCharacterOwing()->GetActorRotation();
}

void UXD_CharacterMovementComponent::SetCharacterRotation(const FRotator& Rotation)
{
	GetCharacterOwing()->SetActorRotation(Rotation);
}

float UXD_CharacterMovementComponent::GetTargetCharacterRotationDifference() const
{
	return UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, GetCharacterRotation()).Yaw;
}

float UXD_CharacterMovementComponent::GetDirection() const
{
	return UKismetMathLibrary::NormalizedDeltaRotator(GetLastVelocityRotation(), GetCharacterRotation()).Yaw;
}

class ACharacter* UXD_CharacterMovementComponent::GetCharacterOwing() const
{
	return CharacterOwner ? CharacterOwner : CastChecked<ACharacter>(GetOwner());
}

float UXD_CharacterMovementComponent::GetMovingOnSlopeSpeedMultiplier() const
{
	if (IsMoving())
	{
		return 1.f + FVector::DotProduct(CurrentFloor.HitResult.Normal, GetVelocity().GetUnsafeNormal()) / 2.f;
	}
	else
	{
		return 1.f;
	}
}

float UXD_CharacterMovementComponent::GetMaxSpeed() const
{
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		return Super::GetMaxSpeed() * GroundMoveSpeedMultiplier * AnimNotifyControlGroundMoveSpeedMultiplier * GetMovingOnSlopeSpeedMultiplier();
	}
	return Super::GetMaxSpeed();
}

FVector UXD_CharacterMovementComponent::CalcAnimRootMotionVelocity(const FVector& RootMotionDeltaMove, float DeltaSeconds, const FVector& CurrentVelocity) const
{
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		return Super::CalcAnimRootMotionVelocity(RootMotionDeltaMove, DeltaSeconds, CurrentVelocity) * GetMovingOnSlopeSpeedMultiplier();
	}
	return Super::CalcAnimRootMotionVelocity(RootMotionDeltaMove, DeltaSeconds, CurrentVelocity);
}
