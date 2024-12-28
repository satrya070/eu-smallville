// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AICharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnhealthChangeAI, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAIDeath);


UCLASS()
class PROJECTSMALLVILLE_API AAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Override the OnConstruction function
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator FacingDirection;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHealth = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(VisibleAnywhere, BlueprintREadOnly, Category="Projectile")
	UStaticMeshComponent* GunMeshComponent;

	UFUNCTION(BlueprintCallable)
	void Fire();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage;

	// still needed?
	UFUNCTION()
	void HandlePlayerDeath();

	UFUNCTION(BlueprintCallable)
	bool IsDead();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float DespawnTime = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Sight = 500.f;

protected:
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AAIProjectile> ProjectileClass;

private:
	UFUNCTION(BlueprintCallable)
	bool PlayerIsAlive();

	FTimerHandle TimerToDestroy;

	void HandleDeath();

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* DeathAnimation;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* GetHitAnimation;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnhealthChangeAI OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAIDeath OnAIDeath;
};
