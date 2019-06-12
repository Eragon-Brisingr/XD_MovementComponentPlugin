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
	None UMETA(Hidden),
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
	Sliding,
	Ragdoll
};

UENUM(BlueprintType)
enum class ECardinalDirection : uint8
{
	North,
	East,
	West,
	South
};
