// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "XD_MovementComponentType.h"
#include "XD_MovementAnimNotify.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "插件_移动_设置根骨骼位移时旋转速度"), abstract)
class XD_MOVEMENTCOMPONENTPLUGIN_API USetRootMotionRotationSpeed : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	USetRootMotionRotationSpeed();
	
	UPROPERTY(EditAnywhere, Category = "移动", meta = (DisplayName = "根骨骼位移时旋转速度"))
	float RootMotionRotationSpeed = 1.f;

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;

	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	virtual FString GetNotifyName_Implementation() const override;
};
