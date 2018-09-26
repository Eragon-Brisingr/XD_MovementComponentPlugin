// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "XD_MovementComponentType.h"
#include "XD_CharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class XD_MOVEMENTCOMPONENTPLUGIN_API UXD_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UXD_CharacterMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void CustomMovingTick(float DeltaTime);

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
protected:
	void UpdateMovementSetting();

	float ChooseMaxWalkSpeed() const;

	float ChooseMaxWalkCrouchedSpeed() const;

	float ChooseMaxAcceleration() const;

	float ChooseBrakingDeceleration() const;

	float ChooseGroundFriction() const;

	float LookingDirectionWithOffsetYaw(float DeltaTime, float OffsetInterpSpeed, float NE, float NW, float SE, float SW, float Buffer);

	float CalculateRotationRate(float SlowSpeed, float SlowSpeedRate, float FastSpeed, float FastSpeedRate) const;

	FVector GetVelocity() const;

	bool CanSprint() const;

	bool HasMovementInput() const;

	FRotator GetLastVelocityRotation() const;

	FRotator GetLastMovementInputRotation() const;

	bool IsMoving() const;
public:
	void SetALS_MovementMode(EALS_MovementMode NewMovementMode);

	void SetGait(ECharacterGait Value);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS State Values")
	ECharacterGait Gait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS State Values")
	ECharacterStanceState StanceState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS State Values")
	uint8 bAiming : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS State Values")
	EALS_MovementMode ALS_MovementMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS State Values")
	ECharacterRotationMode RotationMode;

	FRotator LookingRotation;

	float AimYawDelta;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 bShouldSprint : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 bCanSprint : 1;
public:
	UFUNCTION(BlueprintCallable, Category = "Character Movement")
	FVector GetMovementInput() const;

	UFUNCTION(BlueprintCallable, Category = "Character Movement")
	FRotator GetCharacterRotation() const;

	UFUNCTION(BlueprintCallable, Category = "Character Movement")
	void SetCharacterRotation(const FRotator& Rotation);

	float GetMovementInputVelocityDifference() const;

	float GetTargetCharacterRotationDifference() const;

	float GetDirection() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Rotation System")
	ECardinalDirection CardinalDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Rotation System")
	FRotator TargetRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Rotation System")
	float RotationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Rotation System")
	float RotationRateMultiplier;

	//Config
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walk")
	float WalkingSpeed = 165.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Run")
	float RunningSpeed = 375.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Sprint")
	float SprintingSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Crouch")
	float CrouchingSpeed = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Crouch")
	float CrouchingSprintingSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walk")
	float WalkingAcceleration = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Run")
	float RunningAcceleration = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walk")
	float WalkingDeceleration = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Run")
	float RunningDeceleration = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walk")
	float WalkingGroundFriction = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Run")
	float RunningGroundFriction = 6.f;
};
