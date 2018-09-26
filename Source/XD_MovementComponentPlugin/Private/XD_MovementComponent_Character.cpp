// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_MovementComponent_Character.h"
#include "XD_CharacterMovementComponent.h"


// Sets default values
AXD_MovementComponent_Character::AXD_MovementComponent_Character(const FObjectInitializer& PCIP)
	:Super(PCIP.SetDefaultSubobjectClass<UXD_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{

}

// Called when the game starts or when spawned
void AXD_MovementComponent_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AXD_MovementComponent_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AXD_MovementComponent_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

