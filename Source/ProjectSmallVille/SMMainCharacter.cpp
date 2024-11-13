// Fill out your copyright notice in the Description page of Project Settings.


#include "SMMainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Animation/AnimSequence.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "SMPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"


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
	//UE_LOG(LogTemp, Display, TEXT("world rotation: %s"), *GetActorRotation().ToString());
	//UE_LOG(LogTemp, Display, TEXT("relative rotation: %s"), *GetRootComponent()->GetRelativeRotation().ToString());

	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->AddRelativeRotation(FRotator(0.f, -90.f, 0.f));

	//bUseControllerRotationYaw = false;
	UE_LOG(LogTemp, Display, TEXT("using rotion yaw: %s"), bUseControllerRotationYaw ? TEXT("true") : TEXT("false"));
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bInheritYaw = false;

	// constraint force character to never move on the Y plane! ever!
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 1, 0));
}

// Called every frame
void ASMMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//IsTurning(DeltaTime);
	//UE_LOG(LogTemp, Display, TEXT("%s"), *GetActorRotation().ToString());
	//SmoothRotate(DeltaTime);
	//IsTurning(DeltaTime);
	//SmoothRotate(DeltaTime);
}

// Called to bind functionality to input
void ASMMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// bind the movements to axis
	PlayerInputComponent->BindAxis("MoveForward", this, &ASMMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveBackwards", this, &ASMMainCharacter::MoveBackwards);

	// bind jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASMMainCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASMMainCharacter::StopJump);

	// bind crouch
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASMMainCharacter::Crouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASMMainCharacter::UnCrouch);

	// bind attacks
	PlayerInputComponent->BindAction("Punch", IE_Pressed, this, &ASMMainCharacter::Punch);
	PlayerInputComponent->BindAction("Kick", IE_Pressed, this, &ASMMainCharacter::Kick);
}

void ASMMainCharacter::MoveForward(float Value)
{
	// Forward as in positively in the world X-axis
	if (bIsMovingBackwards || IsHanging)
	{
		return;
	}
	else
	{
		// face forward first if needed
		if (Value > 0.f && GetActorForwardVector().X < 0.f)
		{
			GetController()->SetControlRotation(FRotator(0.f, 0.f, 0.f));
		}

		bIsMovingForward = (Value > 0.0f) ? true : false;

		AddMovementInput(GetActorForwardVector(), Value);
	}	
}

void ASMMainCharacter::MoveBackwards(float Value)
{
	// Backwards as in negatively in the world X-axis
	if (bIsMovingForward || IsHanging)
	{
		return;
	}
	else
	{
		// face backwards first if needed
		if (Value > 0.0f && GetActorForwardVector().X == 1)
		{
			GetController()->SetControlRotation(FRotator(0.f, 180.f, 0.f));
		}

		bIsMovingBackwards = (Value > 0.0f) ? true : false;

		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASMMainCharacter::StartJump()
{
	bPressedJump = true;

	// Cancel punch attack on jump
	if (HandAttackAnimation && (GetMesh()->GetAnimInstance()->Montage_IsPlaying(HandAttackAnimation)))
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, HandAttackAnimation);
	}

	// Cancel kick attack on jump
	if (KickAttackAnimation && (GetMesh()->GetAnimInstance()->Montage_IsPlaying(KickAttackAnimation)))
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, KickAttackAnimation);
	}
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

void ASMMainCharacter::Punch()
{
	if (GetMesh() && HandAttackAnimation && GetMesh()->GetAnimInstance() && !GetCharacterMovement()->IsFalling())
	{
		if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(HandAttackAnimation, 2.0f);
		}
	}
}

void ASMMainCharacter::Kick()
{
	if (GetMesh() && KickAttackAnimation && GetMesh()->GetAnimInstance() && !GetCharacterMovement()->IsFalling())
	{
		if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(KickAttackAnimation);
		}
	}
}

void ASMMainCharacter::IsTurning(float DeltaTime)
{
	FVector world_forward_velocity = GetVelocity();

	if (world_forward_velocity.X < 0.f)
	{
		IsRotating = true;
		DisableMovement();
	}
}


void ASMMainCharacter::SmoothRotate(float DeltaTime)
{
	if (IsRotating == true)
	{
		FRotator CurrentRotation = GetActorRotation();
		FRotator TargetRotation = FRotator(0, 180.f, 0);

		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotateSpeed);
		//UE_LOG(LogTemp, Display, TEXT("setting new rotation: %s"), *GetActorRotation().ToString());
		//UE_LOG(LogTemp, Display, TEXT("actor rotation: %s"), *GetActorRotation().ToString());
		GetController()->SetControlRotation(NewRotation);

		if (CurrentRotation.Equals(NewRotation, 0.01f))
		{
			UE_LOG(LogTemp, Display, TEXT("rotation complete"));
			IsRotating = false;
			EnableMovement();
			//this->SetActorRotation(CurrentRotation.Yaw);
			SetActorRotation(FRotator(0, 180.f, 0));
		}
	}
}

void ASMMainCharacter::EnableMovement()
{
	ASMPlayerController* PlayerController = Cast<ASMPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->SetIgnoreMoveInput(false);
	}
}

void ASMMainCharacter::DisableMovement()
{
	ASMPlayerController* PlayerController = Cast<ASMPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->SetIgnoreMoveInput(true);
	}
}

void ASMMainCharacter::HandleDeath()
{
	Destroy();
}

float ASMMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth -= DamageAmount;

	// call on death
	if (CurrentHealth <= 0)
	{
		// awaits the respawn time before calling destroy(which directly calls respawns)
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ASMMainCharacter::HandleDeath, 2.f, false);
	}

	return DamageAmount;
}

