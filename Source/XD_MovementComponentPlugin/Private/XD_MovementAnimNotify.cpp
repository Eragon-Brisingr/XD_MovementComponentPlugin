// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_MovementAnimNotify.h"
#include <GameFramework/Character.h>
#include <Components/SkeletalMeshComponent.h>
#include "XD_CharacterMovementComponent.h"




USetRootMotionRotationSpeed::USetRootMotionRotationSpeed()
{
	
}

void USetRootMotionRotationSpeed::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		if (UXD_CharacterMovementComponent* MovementComponent = Cast<UXD_CharacterMovementComponent>(Character->GetCharacterMovement()))
		{
			MovementComponent->RootMotionRotationSpeed += RootMotionRotationSpeed;
		}
	}
}

void USetRootMotionRotationSpeed::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		if (UXD_CharacterMovementComponent* MovementComponent = Cast<UXD_CharacterMovementComponent>(Character->GetCharacterMovement()))
		{
			MovementComponent->RootMotionRotationSpeed -= RootMotionRotationSpeed;
		}
	}
}

FString USetRootMotionRotationSpeed::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("根骨骼位移时旋转速度[%s]"), *FString::SanitizeFloat(RootMotionRotationSpeed));
}
