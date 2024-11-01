// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "AIProjectile.generated.h"


UCLASS()
class PROJECTSMALLVILLE_API AAIProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleDefaultsOnly, Category="Projectile")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category="Projectile")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	UStaticMeshComponent* ProjectileMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	UMaterialInstanceDynamic* ProjectileMaterialInstance;

	void FireInDirection(const FVector& FireDirection);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};
