// Fill out your copyright notice in the Description page of Project Settings.


#include "SMMainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Animation/AnimSequence.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
ASMMainCharacter::ASMMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//RootComponent = Root;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

// Called when the game starts or when spawned
void ASMMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("Hello there!"));

	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->AddRelativeRotation(FRotator(0.f, -90.f, 0.f));

	//bUseControllerRotationYaw = false;
	UE_LOG(LogTemp, Display, TEXT("using rotion yaw: %s"), bUseControllerRotationYaw ? TEXT("true") : TEXT("false"));
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bInheritYaw = false;
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
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASMMainCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASMMainCharacter::StopJump);

	// bind crouch
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASMMainCharacter::Crouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASMMainCharacter::UnCrouch);

	PlayerInputComponent->BindAction("Push", IE_Pressed, this, &ASMMainCharacter::Push);
}

void ASMMainCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void ASMMainCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void ASMMainCharacter::StartJump()
{
	bPressedJump = true;
}

void ASMMainCharacter::StopJump()
{
	bPressedJump = false;
}

void ASMMainCharacter::Crouch()
{
	Super::Crouch();
}

void ASMMainCharacter::UnCrouch()
{
	Super::UnCrouch();
}

void ASMMainCharacter::Push()
{
	//PlayAnimSequnce(ShoveAnimation);
	if (GetMesh() && ShoveAnimation && GetMesh()->GetAnimInstance())
	{
		if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(ShoveAnimation);
		}
	}
}

