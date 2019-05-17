// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "XD_MovementComponentFunctionLibrary.generated.h"

UCLASS()
class UXD_MovementComponentFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "移动")
	static FVector GetMovementInput(class ACharacter* Character);

	UFUNCTION(BlueprintPure, Category = "移动")
	static FVector GetPathFollowingInput(class ACharacter* Character);
};
