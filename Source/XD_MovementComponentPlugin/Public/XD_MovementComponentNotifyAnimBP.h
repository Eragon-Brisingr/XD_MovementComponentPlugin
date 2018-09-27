// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "XD_MovementComponentType.h"
#include "XD_MovementComponentNotifyAnimBP.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UXD_MovementComponentNotifyAnimBP : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class XD_MOVEMENTCOMPONENTPLUGIN_API IXD_MovementComponentNotifyAnimBP
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|ALS Value")
	void SetALS_Gait(ECharacterGait CharacterMoveStae);
	virtual void SetALS_Gait_Implementation(ECharacterGait CharacterMoveStae) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|ALS Value")
	void SetALS_Stance(ECharacterStanceState CharacterStanceState);
	virtual void SetALS_Stance_Implementation(ECharacterStanceState CharacterStanceState) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|ALS Value")
	void SetALS_RotationMode(ECharacterRotationMode CharacterRotationMode);
	virtual void SetALS_RotationMode_Implementation(ECharacterRotationMode CharacterRotationMode) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|ALS Value")
	void SetALS_MovementMode(EALS_MovementMode ALS_MovementMode);
	virtual void SetALS_MovementMode_Implementation(EALS_MovementMode ALS_MovementMode) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|ALS Value")
	void SetALS_Aiming(bool Aiming);
	virtual void SetALS_Aiming_Implementation(bool Aiming) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|ALS Value")
	void SetALS_ViewMode(EPlayerViewMode PlayerViewMode);
	virtual void SetALS_ViewMode_Implementation(EPlayerViewMode PlayerViewMode) {}

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|ALS CharacterMovementSettings")
	void SetWalkingSpeed(float WalkingSpeed);
	virtual void SetWalkingSpeed_Implementation(float WalkingSpeed) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|ALS CharacterMovementSettings")
	void SetRunningSpeed(float RunningSpeed);
	virtual void SetRunningSpeed_Implementation(float RunningSpeed) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|ALS CharacterMovementSettings")
	void SetSprintingSpeed(float SprintingSpeed);
	virtual void SetSprintingSpeed_Implementation(float SprintingSpeed) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|ALS CharacterMovementSettings")
	void SetCrouchingSpeed(float CrouchingSpeed);
	virtual void SetCrouchingSpeed_Implementation(float CrouchingSpeed) {}

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|Anim Notify")
	void AnimNotify_Pivot(const FALSN_PivotParams& PivotParams);
	virtual void AnimNotify_Pivot_Implementation(const FALSN_PivotParams& PivotParams) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|Anim Notify")
	void AnimNotify_TurningInPlace(class UAnimMontage* TurnInPlaceMontage, bool ShouldTurnInPlace, bool TurningInPlace, bool TurningRight);
	virtual void AnimNotify_TurningInPlace_Implementation(class UAnimMontage* TurnInPlaceMontage, bool ShouldTurnInPlace, bool TurningInPlace, bool TurningRight) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|Anim Notify")
	void AnimNotify_IdleEntryState(EIdleEntryState IdleEntryState);
	virtual void AnimNotify_IdleEntryState_Implementation(EIdleEntryState IdleEntryState) {}

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS")
	void PlayGetUpAnim(bool FaceDown);
	virtual void PlayGetUpAnim_Implementation(bool FaceDown) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS")
	void SavePoseSnapshot(const FName& PostName);
	virtual void SavePoseSnapshot_Implementation(const FName& PostName) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS")
	void AddCharacterRotation(const FRotator& AddAmount);
	virtual void AddCharacterRotation_Implementation(const FRotator& AddAmount) {}
	
};
