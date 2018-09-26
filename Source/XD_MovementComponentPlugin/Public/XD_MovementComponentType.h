// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XD_MovementComponentType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECharacterGait : uint8
{
	Walking,
	Running,
	Sprinting
};

UENUM(BlueprintType)
enum class ECharacterStanceState : uint8
{
	Standing,
	Crouching
};

UENUM(BlueprintType)
enum class ECharacterRotationMode : uint8
{
	VelocityDirection,
	LookingDirection
};

UENUM(BlueprintType)
enum class EALS_MovementMode : uint8
{
	None,
	Grounded,
	Falling,
	Ragdoll
};

UENUM(BlueprintType)
enum class EPlayerViewMode : uint8
{
	ThirdPerson,
	FirstPerson
};

UENUM(BlueprintType)
enum class ECardinalDirection : uint8
{
	North,
	East,
	West,
	South
};

UENUM(BlueprintType)
enum class EIdleEntryState : uint8
{
	N_Idle,
	LF_Idle,
	C_Idle
};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Forwards,
	Backwards
};

USTRUCT(BlueprintType)
struct FALSN_PivotParams
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS_PivotParams")
	float PivotDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS_PivotParams")
	EMovementDirection CompletedMovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS_PivotParams")
	float CompletedStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS_PivotParams")
	EMovementDirection InterruptedMovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS_PivotParams")
	float InterruptedStartTime;
};

