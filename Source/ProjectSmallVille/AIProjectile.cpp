// Fill out your copyright notice in the Description page of Project Settings.


#include "AIProjectile.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAIProjectile::AAIProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetCollisionProfileName(TEXT("BoxCollision"));
	BoxCollider->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

	// collider can be directly set as root, no need for scenecomponent container
	RootComponent = BoxCollider;

	if (!ProjectileMovementComponent)
	{
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(BoxCollider);
		ProjectileMovementComponent->InitialSpeed = 100.f;
		ProjectileMovementComponent->MaxSpeed = 100.f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 3.0f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	// how long the projectile to exist in seconds
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AAIProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	BoxCollider->OnComponentHit.AddDynamic(this, &AAIProjectile::OnHit);
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

void AAIProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && !OtherComponent->IsSimulatingPhysics())
	{
		//OtherComponent->AddImpulseAtLocation()

		if (OtherActor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(OtherActor, DamagePoints, nullptr, this, UDamageType::StaticClass());
			UE_LOG(LogTemp, Warning, TEXT("Projectile: did 10 damage!"));
		}
	}
	Destroy();
}

