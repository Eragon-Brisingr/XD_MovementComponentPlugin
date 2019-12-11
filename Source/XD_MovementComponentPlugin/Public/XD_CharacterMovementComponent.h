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

	void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const override;

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void BeginPlay() override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void CustomMovingTick(float DeltaTime);

	void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
protected:
	void UpdateMovementSetting();

	float ChooseMaxWalkSpeed() const;

	float ChooseMaxWalkCrouchedSpeed() const;

	float ChooseMaxWalkAcceleration() const;

	float ChooseWalkBrakingDeceleration() const;

	float ChooseGroundFriction() const;

	float LookingDirectionWithOffsetYaw(float DeltaTime, float OffsetInterpSpeed, float NE, float NW, float SE, float SW, float Buffer);

	float CalculateRotationRate(float SlowSpeed, float SlowSpeedRate, float FastSpeed, float FastSpeedRate) const;

	UFUNCTION(BlueprintCallable, Category = "角色|移动")
	FVector GetVelocity() const;

	UFUNCTION(BlueprintCallable, Category = "角色|移动")
	bool CanSprint() const;

	UFUNCTION(BlueprintCallable, Category = "角色|移动")
	bool HasMovementInput() const;

	UFUNCTION(BlueprintCallable, Category = "角色|移动")
	FRotator GetLastVelocityRotation() const;

	UFUNCTION(BlueprintCallable, Category = "角色|移动")
	FRotator GetLastMovementInputRotation() const;

	UFUNCTION(BlueprintCallable, Category = "角色|移动")
	bool IsMoving() const;
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnALS_MovementModeChanged, EALS_MovementMode, PreALS_MovementMode, EALS_MovementMode, ALS_MovementMode);
	UPROPERTY(BlueprintAssignable)
	FOnALS_MovementModeChanged OnALS_MovementModeChanged;
	void SetALS_MovementMode(EALS_MovementMode NewMovementMode);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInvokeGaitChanged, ECharacterGait, PreGait, ECharacterGait, Gait);
	UPROPERTY(BlueprintAssignable)
	FOnInvokeGaitChanged OnGaitChanged;
	void SetGait(ECharacterGait Value);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStanceStateChanged, ECharacterStanceState, PreStanceState, ECharacterStanceState, StanceState);
	UPROPERTY(BlueprintAssignable)
	FOnStanceStateChanged OnStanceStateChanged;
	void SetStanceState(ECharacterStanceState Value);

	virtual void Crouch(bool bClientSimulation) override;

	virtual void UnCrouch(bool bClientSimulation)  override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRotationModeChanged, ECharacterRotationMode, PreRotationMode, ECharacterRotationMode, RotationMode);
	UPROPERTY(BlueprintAssignable)
	FOnRotationModeChanged OnRotationModeChanged;
	void SetRotationMode(ECharacterRotationMode Value);
public:
	UPROPERTY(BlueprintReadOnly, Category = "角色|移动|状态")
	ECharacterGait CurrentGait;

	UPROPERTY(BlueprintReadOnly, Category = "角色|移动|状态")
	ECharacterStanceState StanceState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|状态")
	uint8 bAiming : 1;

	UPROPERTY(BlueprintReadOnly, Category = "角色|移动|状态")
	EALS_MovementMode ALS_MovementMode = EALS_MovementMode::Grounded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色|移动|状态")
	ECharacterRotationMode RotationMode = ECharacterRotationMode::LookingDirection;

	UPROPERTY(EditAnywhere, Category = "角色|移动|状态")
	uint8 bAutoUpdateControlRotation : 1;

	UPROPERTY(BlueprintReadOnly, Category = "角色|移动", Replicated)
	FRotator ControlRotation;
	UPROPERTY(BlueprintReadOnly, Category = "角色|移动")
	float AimYawDelta;
public:
	UFUNCTION(BlueprintPure, Category = "角色|移动|状态")
	bool IsSprinting() const;
public:
	UPROPERTY(BlueprintReadOnly, Category = "角色|移动|状态", Replicated)
	FVector MovementInput;

	UFUNCTION(BlueprintCallable, Category = "角色|移动")
	FVector GetMovementInput() const;

	UFUNCTION(BlueprintCallable, Category = "角色|移动")
	FRotator GetCharacterRotation() const;

	UFUNCTION(BlueprintCallable, Category = "角色|移动")
	void SetCharacterRotation(const FRotator& Rotation);

	UPROPERTY(BlueprintReadOnly, Category = "角色|移动")
	float MovementInputVelocityDifference;

	UFUNCTION(BlueprintCallable, Category = "角色|移动")
	float GetTargetCharacterRotationDifference() const;

	UFUNCTION(BlueprintCallable, Category = "角色|移动")
	float GetDirection() const;

	UFUNCTION(BlueprintCallable, Category = "角色|移动")
	class ACharacter* GetCharacterOwing() const;

	//CharacterRotation
public:
	UPROPERTY(BlueprintReadOnly, Category = "角色|移动|转身")
	ECardinalDirection CardinalDirection;

	UPROPERTY(BlueprintReadOnly, Category = "角色|移动|转身")
	FRotator TargetRotation;

	UPROPERTY(BlueprintReadOnly, Category = "角色|移动|转身")
	float RotationOffset;

	UPROPERTY(BlueprintReadOnly, Category = "角色|移动|转身")
	float RotationRateMultiplier = 1.f;

	float RootMotionRotationSpeed = 0.f;
public:
	UPROPERTY(BlueprintReadWrite, Category = "角色|移动|高级")
	float GroundMoveSpeedMultiplier = 1.f;

	UFUNCTION(BlueprintCallable, Category = "角色|移动|高级")
	float GetMovingOnSlopeSpeedMultiplier() const;

	virtual float GetMaxSpeed() const override;

	virtual FVector CalcAnimRootMotionVelocity(const FVector& RootMotionDeltaMove, float DeltaSeconds, const FVector& CurrentVelocity) const override;
	//Config
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|行走")
	float WalkingSpeed = 165.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|奔跑")
	float RunningSpeed = 375.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|冲刺")
	float SprintingSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|蹲下")
	float CrouchingSpeed = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|蹲下")
	float CrouchingSprintingSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|行走")
	float WalkingAcceleration = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|奔跑")
	float RunningAcceleration = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|行走")
	float WalkingDeceleration = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|奔跑")
	float RunningDeceleration = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|行走")
	float WalkingGroundFriction = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|奔跑")
	float RunningGroundFriction = 6.f;
	
	//滑落
public:
	UPROPERTY(EditAnywhere, Category = "角色|移动|滑落")
	float SlidableFloorAngle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "角色|移动|滑落")
	float SlidableFloorZ;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "角色|移动|滑落")
	float SlidableFloorNormalZ;
	UFUNCTION(BlueprintCallable, Category = "角色|移动|滑落")
	void SetSlidableFloorAngle(float Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|滑落")
	float MaxSlideSpeed = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "角色|移动|滑落")
	float SlideAcceleration = 980.f;

	void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;

	float VisualizeMovement() const override;

	UFUNCTION(BlueprintCallable, Category = "角色|移动|滑落")
	bool IsSliding() const;
	//防止向可滑落斜坡移动时的抖动
	float PreWorldLocationZ;
	float CurPrepareSlidingOffsetZ;
	//已经在进行滑动判定了，但由于还有向上的移动速度所以不会真正进入滑动状态
	UFUNCTION(BlueprintCallable, Category = "角色|移动|滑落")
	bool IsPrepareSliding() const;
	UFUNCTION(BlueprintCallable, Category = "角色|移动|滑落")
	float GetSlideSpeedWeight() const;
	UFUNCTION(BlueprintCallable, Category = "角色|移动|滑落")
	FVector GetSlideDir() const;

	UFUNCTION(BlueprintCallable, Category = "角色|移动")
	FVector GetFloorDir() const;
};
