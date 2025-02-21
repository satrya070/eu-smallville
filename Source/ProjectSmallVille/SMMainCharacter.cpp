// Fill out your copyright notice in the Description page of Project Settings.


#include "SMMainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Animation/AnimSequence.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
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

	if (bIsRotating == true && !IsDead())
	{
		SmoothRotateTo(DeltaTime);
	}

	// Keeps track of player is jumping status
	HandleJumpState();

	// Handle the smooth capsule shrinking/growing based on player crouch/jump positioning
	InterpolateCapsuleHeight(DeltaTime, GetCapsuleHalfHeight());

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
		// dont allow forward while backwards key is pressed
		return;
	}
	else
	{
		// call rotation to front first if facing backwards(yaw 180)
		if (Value > 0.f && FacingDirection.Yaw > 179.f)
		{
			bIsRotating = true;
			FacingDirection = FRotator(0.f, 0.f, 0.f);
			CancelAnimation(GetHitAnimation);
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
		// dont allow backwards while forward key is pressed
		return;
	}
	else
	{
		// call rotation first if facing forward(yaw 0)
		if (Value > 0.0f && FacingDirection.Yaw < 1.f)
		{
			bIsRotating = true;
			FacingDirection = FRotator(0.f, 180.f, 0.f);
			CancelAnimation(GetHitAnimation);
		}

		bIsMovingBackwards = (Value > 0.0f) ? true : false;

		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASMMainCharacter::StartJump()
{
	bPressedJump = true;

	if (JumpSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), JumpSound, GetActorLocation());
	}

	// Cancel punch/kick attacks on jump
	CancelAnimation(HandAttackAnimation);
	CancelAnimation(KickAttackAnimation);
}

void ASMMainCharacter::StopJump()
{
	bPressedJump = false;
}

void ASMMainCharacter::Crouch()
{
	//Super::Crouch();
	bIsCrouching = true;
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
}

void ASMMainCharacter::UnCrouch()
{
	//Super::UnCrouch();
	bIsCrouching = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	//UE_LOG(LogTemp, Display, TEXT("release crouch"));
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

	/* TODO delete(tried fic camera view issue on game over menu)
	ASMPlayerController* PlayerController = Cast<ASMPlayerController>(GetController());
	if (PlayerController)
	{
		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
		if (CameraManager)
		{
			CameraManager->Set(FRotator(0.0f, -90.f, 0.0f));
		}
	}*/

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

void ASMMainCharacter::CancelAnimation(UAnimMontage* PlayingAnimation)
{
	if (PlayingAnimation && (GetMesh()->GetAnimInstance()->Montage_IsPlaying(PlayingAnimation)))
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, PlayingAnimation);

		// On end of the animNotify, speed can be temporarily set to 100.f
		// this needs to be restored to 600.f on an interrupted animation as well
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
}

void ASMMainCharacter::HandleJumpState()
{
	if (GetCharacterMovement()->IsFalling())
	{
		bIsJumping = true;
	}
	else
	{
		bIsJumping = false;
	}
}

void ASMMainCharacter::InterpolateCapsuleHeight(float DeltaTime, float TargetHeight)
{
	float CurrentHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float Tolerance = .5f;
	if (FMath::Abs(CurrentHeight - TargetHeight) > Tolerance)
	{
		float UpdatedHeight = FMath::FInterpTo(CurrentHeight, TargetHeight, DeltaTime, 5.f);
		GetCapsuleComponent()->SetCapsuleHalfHeight(UpdatedHeight);
	}
}

float ASMMainCharacter::GetCapsuleHalfHeight()
{
	// return which height the capsule should be depending on the player position status
	if (bIsJumping)
	{
		return JumpCapsuleHalfHeight;
	}

	if (bIsCrouching)
	{
		return CrouchedCapsuleHalfHeight;
	}

	return StandingCapsuleHalfHeight;
}

float ASMMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth -= DamageAmount;

	// dispatch event to update lifebar GUI (among other)
	HealthChanged.Broadcast(CurrentHealth);

	// play hit montage
	if (GetMesh() && GetHitAnimation && GetMesh()->GetAnimInstance() && !GetCharacterMovement()->IsFalling() && !IsDead())
	{
		// cancel punch/kick animation if getting hit & properly reset walk speed
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(HandAttackAnimation) || GetMesh()->GetAnimInstance()->Montage_IsPlaying(KickAttackAnimation))
		{
			GetCharacterMovement()->MaxWalkSpeed = 600.f;
		}

		GetMesh()->GetAnimInstance()->Montage_Play(GetHitAnimation);
	}

	if (GetHitSound && !IsDead())
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GetHitSound, GetActorLocation());
	}

	// call on death
	if (CurrentHealth <= 0)
	{
		// play death animation
		/*if (GetMesh() && GetMesh()->GetAnimInstance() && DieAnimation && !GetCharacterMovement()->IsFalling())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(DieAnimation);
		}*/

		// plays immediately upon dead
		if (DeadSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeadSound, GetActorLocation());
		}

		// dont allow movementinput when dead 
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		// awaits the respawn time before calling destroy(which directly calls respawns)
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ASMMainCharacter::HandleDeath, 2.f, false);
	}

	return DamageAmount;
}

bool ASMMainCharacter::IsDead()
{
	if (CurrentHealth <= 0.f)
	{
		return true;
	}
	return false;
}

