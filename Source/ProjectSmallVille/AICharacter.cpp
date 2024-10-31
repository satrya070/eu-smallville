// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "Logging/LogMacros.h"
#include "AIProjectile.h"

// Sets default values
AAICharacter::AAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AAICharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	Health = Health - DamageAmount;
	//UE_LOG(LogTemp, Display, TEXT("AI Health is: %s"), Health);

	if (Health <= 0)
	{
		Destroy();
	}

	return DamageAmount;
}

void AAICharacter::Fire()
{
	if (ProjectileClass)
	{
		// TODO set muzzle offset to spawn slighty in front

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			FVector ProjectileSpawnLocation = GetActorLocation() + MuzzleOffset;
			FRotator MuzzleRotation = FRotator(0.f, 0.f, 0.f);

			// spawn projectile at muzzle
			AAIProjectile* Projectile = World->SpawnActor<AAIProjectile>(ProjectileClass, ProjectileSpawnLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				FVector LaunchDirection = MuzzleRotation.Vector();				
				Projectile->FireInDirection(GetActorForwardVector());
			}
		}
	}
}


