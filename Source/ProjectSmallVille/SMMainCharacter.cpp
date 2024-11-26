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
#include "Kismet/KismetMathLibrary.h"


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

	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->AddRelativeRotation(FRotator(0.f, -90.f, 0.f));

	//bUseControllerRotationYaw = false;
	//UE_LOG(LogTemp, Display, TEXT("using rotion yaw: %s"), bUseControllerRotationYaw ? TEXT("true") : TEXT("false"));
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bInheritYaw = false;

	// constraint force character to never move on the Y plane! ever!
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 1, 0));

	// bind animation montage function on interupts
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		//AnimInstance->OnMontageBlendingOut.AddDynamic(this, &ASMMainCharacter::OnMontageEnded);
		AnimInstance->OnMontageEnded.AddDynamic(this, &ASMMainCharacter::OnMontageEnded);
		UE_LOG(LogTemp, Display, TEXT("anim stuff bounded"));
	}
}

// Called every frame
void ASMMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRotating == true)
	{
		SmoothRotateTo(DeltaTime);
	}
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
		// call rotation to front first if facing backwards(yaw 180)
		if (Value > 0.f && FacingDirection.Yaw > 179.f)
		{
			//GetController()->SetControlRotation(FRotator(0.f, 0.f, 0.f));
			bIsRotating = true;
			FacingDirection = FRotator(0.f, 0.f, 0.f);
			//UE_LOG(LogTemp, Display, TEXT("Rotating"));
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
		// call rotation first if facing forward(yaw 0)
		if (Value > 0.0f && FacingDirection.Yaw < 1.f)
		{
			//GetController()->SetControlRotation(FRotator(0.f, 180.f, 0.f));
			bIsRotating = true;
			FacingDirection = FRotator(0.f, 180.f, 0.f);
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
		
		// On end of the animNotify, speed is temporarily decreased to 100.f
		// this needs to be set back to 600.f on an interrupted cancel animation
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}

	// Cancel kick attack on jump
	if (KickAttackAnimation && (GetMesh()->GetAnimInstance()->Montage_IsPlaying(KickAttackAnimation)))
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, KickAttackAnimation);

		// On end of the animNotify, speed is temporarily decreased to 100.f
		// this needs to be set back to 600.f on an interrupted cancel animation
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
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
	OnPlayerDeath.Broadcast();
	Destroy();
}

void ASMMainCharacter::SmoothRotateTo(float DeltaTime)
{
	FRotator CurrentRotation = GetActorRotation();
	float Tolerance = 1.0f;

	if (FMath::Abs(CurrentRotation.Yaw - FacingDirection.Yaw) > Tolerance)
	{
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, FacingDirection, DeltaTime, RotateSpeed);
		SetActorRotation(NewRotation);
	}
	else
	{
		// UE_LOG(LogTemp, Display, TEXT("Done rotating"));
		bIsRotating = false;
	}
}

void ASMMainCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// TODO only has to lock on punch&kick montage
	bLockDamage = false;

	//UE_LOG(LogTemp, Display, TEXT("Damage Locked again"));
	UE_LOG(LogTemp, Display, TEXT("Damage was locked, finished playing Anim: '%s'"), *Montage->GetFName().ToString());
}

float ASMMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth -= DamageAmount;

	// play hit montage
	if (GetMesh() && GetHitAnimation && GetMesh()->GetAnimInstance() && !GetCharacterMovement()->IsFalling())
	{
		// cancel punch/kick animation if getting hit & properly reset walk speed
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(HandAttackAnimation) || GetMesh()->GetAnimInstance()->Montage_IsPlaying(KickAttackAnimation))
		{
			GetCharacterMovement()->MaxWalkSpeed = 600.f;
		}

		GetMesh()->GetAnimInstance()->Montage_Play(GetHitAnimation);
	}

	// call on death
	if (CurrentHealth <= 0)
	{
		// awaits the respawn time before calling destroy(which directly calls respawns)
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ASMMainCharacter::HandleDeath, 2.f, false);
	}

	return DamageAmount;
}

