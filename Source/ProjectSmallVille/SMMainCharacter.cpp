// Fill out your copyright notice in the Description page of Project Settings.


#include "SMMainCharacter.h"

// Sets default values
ASMMainCharacter::ASMMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASMMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("Hello there!"));
}

// Called every frame
void ASMMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASMMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// bind the movements to axis
	PlayerInputComponent->BindAxis("MoveForward", this, &ASMMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASMMainCharacter::MoveRight);

	// bind jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASMMainCharacter::Jump);
}

void ASMMainCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void ASMMainCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void ASMMainCharacter::Jump()
{

}

