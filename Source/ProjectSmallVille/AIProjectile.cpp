// Fill out your copyright notice in the Description page of Project Settings.


#include "AIProjectile.h"
#include "Kismet/GameplayStatics.h"

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
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		CollisionComponent->InitSphereRadius(15.f);
		CollisionComponent->OnComponentHit.AddDynamic(this, &AAIProjectile::OnHit);

		// sets the rootcomponent to be the collisioncomponent
		RootComponent = CollisionComponent;
	}

	if (!ProjectileMovementComponent)
	{
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 100.f;
		ProjectileMovementComponent->MaxSpeed = 100.f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 3.0f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	if (!ProjectileMeshComponent)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/AIBlueprints/meshes/Sphere.Sphere'"));
		if (Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}

		static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("'/Game/AIBlueprints/meshes/Fbx_Default_Material.Fbx_Default_Material'"));
		if (Material.Succeeded())
		{
			ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
		}
		ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
		ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
		ProjectileMeshComponent->SetupAttachment(RootComponent);

	}

	// how long the projectile to exist in seconds
	InitialLifeSpan = 3.0f;
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

