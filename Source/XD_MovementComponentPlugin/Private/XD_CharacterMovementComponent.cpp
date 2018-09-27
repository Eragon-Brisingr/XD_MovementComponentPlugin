// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_CharacterMovementComponent.h"
#include "XD_MovementComponentFunctionLibrary.h"
#include <GameFramework/Character.h>
#include <Kismet/KismetMathLibrary.h>


UXD_CharacterMovementComponent::UXD_CharacterMovementComponent()
	:bCanSprint(true)
{
	RotationRate = FRotator::ZeroRotator;

	MovementState.bCanCrouch = true;
	MovementState.bCanSwim = true;
	MovementState.bCanFly = true;
}

void UXD_CharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	CustomMovingTick(DeltaTime);

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UXD_CharacterMovementComponent::CustomMovingTick(float DeltaTime)
{
	float PreAimYaw = LookingRotation.Yaw;
	LookingRotation = CharacterOwner->GetControlRotation();
	AimYawRate = (LookingRotation.Yaw - PreAimYaw) / DeltaTime;
	AimYawDelta = UKismetMathLibrary::NormalizedDeltaRotator(LookingRotation, GetCharacterRotation()).Yaw;

	switch (ALS_MovementMode)
	{
	case EALS_MovementMode::Grounded:
		if (StanceState == ECharacterStanceState::Standing)
		{
			switch (InvokeGait)
			{
			case ECharacterGait::Running:
			case ECharacterGait::Sprinting:
				float MovementInputVelocityDifference = FMath::Abs(GetMovementInputVelocityDifference());
				MaxAcceleration = RunningAcceleration * UKismetMathLibrary::MapRangeClamped(MovementInputVelocityDifference, 45.f, 130.f, 1.f, 0.2f);
				GroundFriction = RunningGroundFriction * UKismetMathLibrary::MapRangeClamped(MovementInputVelocityDifference, 45.f, 130.f, 1.f, 0.4f);
				break;
			}
		}
		break;
	case EALS_MovementMode::Ragdoll:
		//TODO Do While Ragdolling
		break;
	}

	if (CharacterOwner->IsLocallyControlled())
	{
		if (bShouldSprint)
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
		else
		{
			SetGait(ECharacterGait::Running);
		}

		switch (ALS_MovementMode)
		{
		case EALS_MovementMode::Grounded:
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
			else if (!CharacterOwner->IsPlayingRootMotion() && RotationMode == ECharacterRotationMode::LookingDirection)
			{
				auto LimitRotation = [&, this](float AimYawLimit, float InterpSpeed)
				{
					if (FMath::Abs(AimYawDelta) > AimYawLimit)
					{
						FRotator CharacterRotation = GetCharacterRotation();
						float Yaw = AimYawDelta > 0.f ? LookingRotation.Yaw - AimYawLimit : LookingRotation.Yaw + AimYawLimit;
						TargetRotation = FRotator(CharacterRotation.Pitch, Yaw, CharacterRotation.Roll);
						SetCharacterRotation(FMath::RInterpTo(CharacterRotation, TargetRotation, DeltaTime, InterpSpeed));
					}
				};
				if (bAiming)
				{
					LimitRotation(90.f, 15.f);
				}
				// 				else
				// 				{
				// 					if (PlayerViewMode == EPlayerViewMode::FirstPerson)
				// 					{
				// 						LimitRotation(90.f, 15.f);
				// 					}
				// 				}
			}
			break;

		case EALS_MovementMode::Falling:

			break;
		}
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
	//TODO 存在争议
	if (bAiming)
	{
		switch (InvokeGait)
		{
		case ECharacterGait::Walking:
		case ECharacterGait::Running:
			return WalkingSpeed;
		case ECharacterGait::Sprinting:
			return RunningSpeed;
		}
	}
	else
	{
		switch (InvokeGait)
		{
		case ECharacterGait::Walking:
			return WalkingSpeed;
		case ECharacterGait::Running:
			return RunningSpeed;
		case ECharacterGait::Sprinting:
			return SprintingSpeed;
		}
	}
	return 0.f;
}

float UXD_CharacterMovementComponent::ChooseMaxWalkCrouchedSpeed() const
{
	switch (InvokeGait)
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
	switch (InvokeGait)
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
	switch (InvokeGait)
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
	switch (InvokeGait)
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
		return CharacterOwner->GetVelocity();
	case EALS_MovementMode::Ragdoll:
		return CharacterOwner->GetMesh()->GetComponentVelocity();
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
	if (Value != InvokeGait)
	{
		ECharacterGait PreInvokeGait = InvokeGait;
		InvokeGait = Value;
		UpdateMovementSetting();
		OnInvokeGaitChanged.Broadcast(PreInvokeGait, InvokeGait);
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

FVector UXD_CharacterMovementComponent::GetMovementInput() const
{
	return UXD_MovementComponentFunctionLibrary::GetMovementInput(CharacterOwner);
}

FRotator UXD_CharacterMovementComponent::GetCharacterRotation() const
{
	return CharacterOwner->GetActorRotation();
}

void UXD_CharacterMovementComponent::SetCharacterRotation(const FRotator& Rotation)
{
	CharacterOwner->SetActorRotation(Rotation);
}

float UXD_CharacterMovementComponent::GetMovementInputVelocityDifference() const
{
	return UKismetMathLibrary::NormalizedDeltaRotator(GetLastMovementInputRotation(), GetLastVelocityRotation()).Yaw;
}

float UXD_CharacterMovementComponent::GetTargetCharacterRotationDifference() const
{
	return UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, GetCharacterRotation()).Yaw;
}

float UXD_CharacterMovementComponent::GetDirection() const
{
	return UKismetMathLibrary::NormalizedDeltaRotator(GetLastVelocityRotation(), GetCharacterRotation()).Yaw;
}
