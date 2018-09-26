// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_CharacterMovementComponent.h"
#include "XD_MovementComponentFunctionLibrary.h"




FVector UXD_CharacterMovementComponent::GetMovementInput(class ACharacter* Character) const
{
	return UXD_MovementComponentFunctionLibrary::GetMovementInput(Character);
}
