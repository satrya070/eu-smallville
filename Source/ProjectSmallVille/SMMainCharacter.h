// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SMMainCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);

UCLASS()
class PROJECTSMALLVILLE_API ASMMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASMMainCharacter();

	UPROPERTY(EditAnywhere, Category="Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, Category="Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, Category="Attacks")
	UAnimMontage* HandAttackAnimation;

	UPROPERTY(EditAnywhere, Category = "Attacks")
	UAnimMontage* KickAttackAnimation;

	UPROPERTY(EditAnywhere, Category = "Attacks")
	UAnimMontage* GetHitAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerDeath OnPlayerDeath;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveBackwards(float Value);

	void StartJump();
	void StopJump();

	void Crouch();
	void UnCrouch();

	void Punch();
	void Kick();

	UPROPERTY(BlueprintReadOnly)
	bool bIsRotating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsHanging;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLockDamage;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY(EditAnywhere)
	float RotateSpeed = 10.f;

	bool bIsMovingForward;
	bool bIsMovingBackwards;

	void EnableMovement();
	void DisableMovement();

	FTimerHandle RespawnTimerHandle;

	UFUNCTION(BlueprintCallable)
	void HandleDeath();

	void SmoothRotateTo(float DeltaTime);

	// yaw of 0.f being front(facing right), 180.f facing back(facing left)
	FRotator FacingDirection = FRotator(0.f, 0.f, 0.f);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
