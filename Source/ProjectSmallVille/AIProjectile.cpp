// Fill out your copyright notice in the Description page of Project Settings.


#include "AIProjectile.h"

// Sets default values
AAIProjectile::AAIProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile"));
	}

	if (!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileComponent"));
		CollisionComponent->InitSphereRadius(15.f);
		// sets the rootcomponent to be the collisioncomponent
		RootComponent = CollisionComponent;
	}

	if (!ProjectileMovementComponent)
	{
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 3000.f;
		ProjectileMovementComponent->MaxSpeed = 3000.f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 3.0f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}
}

// Called when the game starts or when spawned
void AAIProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIProjectile::FireInDirection(const FVector& FireDirection)
{
	ProjectileMovementComponent->Velocity = FireDirection * ProjectileMovementComponent->InitialSpeed;
}

