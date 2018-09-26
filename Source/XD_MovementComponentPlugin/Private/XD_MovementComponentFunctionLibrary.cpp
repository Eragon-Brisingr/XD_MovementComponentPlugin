// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "XD_MovementComponentFunctionLibrary.h"
#include <GameFramework/Character.h>

FVector UXD_MovementComponentFunctionLibrary::GetMovementInput(class ACharacter* Character)
{
	return Character ? Character->GetCharacterMovement()->GetCurrentAcceleration() : FVector::ZeroVector;
}
