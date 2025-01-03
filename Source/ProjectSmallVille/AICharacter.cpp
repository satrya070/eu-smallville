// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "Logging/LogMacros.h"
#include "AIProjectile.h"
#include "Engine/staticMeshSocket.h"
#include "SMMainCharacter.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AAICharacter::AAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMeshComponent"));
	if (GetMesh())
	{
		GunMeshComponent->SetupAttachment(GetMesh());
	}
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();

	UStaticMesh* GunStaticMesh = GunMeshComponent->GetStaticMesh();

	//ASMMainCharacter* Player = Cast<ASMMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	//if (Player) {
	//	Player->OnPlayerDeath.AddDynamic(this, &AAICharacter::HandlePlayerDeath);
	//}

	// set facing direction(if needed)
}

void AAICharacter::OnConstruction(const FTransform& Transform)
{
	if (GetMesh()->DoesSocketExist(TEXT("hand_r")))
	{
		GunMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("hand_r"));
	}
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
	
	//UE_LOG(LogTemp, Display, TEXT("AI take damage"));
	if (GetMesh() && GetMesh()->GetAnimInstance() && GetHitAnimation)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(GetHitAnimation, 1.f);
	}

	Health = Health - DamageAmount;

	// dispatch event for damage to be consumed in GUI (and other)
	OnHealthChanged.Broadcast(Health);

	if (Health <= 0)
	{
		OnAIDeath.Broadcast();

		// play dead anim
		//* this only for CharacterAI since adding via a statemachine wasnt possible due to
		// skeletal problems, so this and the despawn time is a hack.
		if (GetMesh() && GetMesh()->GetAnimInstance() && DeathAnimation)
		{
			GetMesh()->GetAnimInstance()->Montage_Play(DeathAnimation);
		}

		GetWorld()->GetTimerManager().SetTimer(TimerToDestroy, this, &AAICharacter::HandleDeath, DespawnTime, false);
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

			// Only the X offset gets mirrored, X & Y stay the same on both sides to keep it on the same height and distance
			FVector ProjectileSpawnLocation = (FacingDirection.Yaw < 1.f) ? (GetActorLocation() + FVector(MuzzleOffset.X, MuzzleOffset.Y, MuzzleOffset.Z)) : (GetActorLocation() - FVector(MuzzleOffset.X, -MuzzleOffset.Y, -MuzzleOffset.Z));				
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

void AAICharacter::HandlePlayerDeath()
{
	UE_LOG(LogTemp, Display, TEXT("Handle Player dead*"));
}

bool AAICharacter::IsDead()
{
	if (Health <= 0.f)
	{
		return true;
	}
	return false;
}

bool AAICharacter::PlayerIsAlive()
{
	ASMMainCharacter* Player = Cast<ASMMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player)
	{
		if (Player->CurrentHealth > 0.f)
		{
			return true;
		}
	}
	return false;
}

void AAICharacter::HandleDeath()
{
	Destroy();
}
