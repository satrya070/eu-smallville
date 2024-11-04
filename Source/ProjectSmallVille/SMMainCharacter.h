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
	UAnimMontage* ShoveAnimation;

	UPROPERTY(EditAnywhere)
	float Maxhealth = 100.f;

	UPROPERTY(VisibleAnywhere)
	float CurrentHealth;


	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	UStaticMesh* MeshComp;*/


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
	void MoveRight(float Value);

	void StartJump();
	void StopJump();

	void Crouch();
	void UnCrouch();

	// attacks
	void Push();
	void IsTurning(float DeltaTime);

	void SmoothRotate(float DeltaTime);

	UPROPERTY(BlueprintReadOnly)
	bool IsRotating;

private:
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere)
	float RotateSpeed = 2.5f;

	bool bIsMovingForward;
	bool bIsMovingBackwards;

	void EnableMovement();
	void DisableMovement();
};
