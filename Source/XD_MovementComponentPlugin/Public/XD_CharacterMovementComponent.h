// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "XD_CharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class XD_MOVEMENTCOMPONENTPLUGIN_API UXD_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 bShouldSprint : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 bCanSprint : 1;
public:
	FVector GetMovementInput(class ACharacter* Character) const;


};
