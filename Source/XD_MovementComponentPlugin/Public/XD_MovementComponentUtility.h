// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(XD_MovementComponent_Log, Log, All);

#define MovementComponent_Display_LOG(FMT, ...) UE_LOG(XD_MovementComponent_Log, Display, TEXT(FMT), ##__VA_ARGS__)
#define MovementComponent_Warning_LOG(FMT, ...) UE_LOG(XD_MovementComponent_Log, Warning, TEXT(FMT), ##__VA_ARGS__)
#define MovementComponent_Error_Log(FMT, ...) UE_LOG(XD_MovementComponent_Log, Error, TEXT(FMT), ##__VA_ARGS__)
