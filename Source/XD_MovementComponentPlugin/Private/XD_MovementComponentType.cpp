// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_MovementComponentType.h"

#define LOCTEXT_NAMESPACE "MovementType"

FText FMovementTypeUtils::GetGaitName(ECharacterGait Gait)
{
	switch (Gait)
	{
	case ECharacterGait::Walking:
		return LOCTEXT("行走", "行走");
	case ECharacterGait::Running:
		return LOCTEXT("行走", "奔跑");
	case ECharacterGait::Sprinting:
		return LOCTEXT("行走", "冲刺");
	}
	checkNoEntry();
	return FText::GetEmpty();
}

#undef LOCTEXT_NAMESPACE
