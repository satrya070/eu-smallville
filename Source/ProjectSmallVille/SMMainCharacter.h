// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SMMainCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//virtual void OnStartCrouch override;

	/*virtual void NotifyHit(
		class UPrimitiveComponent* MyComp,
		AActor* Other,
		class UPrimitiveComponent* OtherComp,
		bool bSelfMoved,
		FVector HitLocation,
		FVector HitNormal,
		FVector NormalImpulse,
		const FHitResult& Hit
	) override;*/

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
	void IsTurning(float DeltaTime);

	void SmoothRotate(float DeltaTime);

	UPROPERTY(BlueprintReadOnly)
	bool IsRotating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsHanging;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY(EditAnywhere)
	float RotateSpeed = 2.5f;

	bool bIsMovingForward;
	bool bIsMovingBackwards;

	void EnableMovement();
	void DisableMovement();

	FTimerHandle RespawnTimerHandle;
	void HandleDeath();
};
