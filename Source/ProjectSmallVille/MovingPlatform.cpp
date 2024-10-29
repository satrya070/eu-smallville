// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();

	TargetLocation = StartLocation + TargetOffset;

	UE_LOG(LogTemp, Display, TEXT("target is: %f"), TargetLocation.X);
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FVector::Dist(GetActorLocation(), TargetLocation) > 1.f)
	{
		FVector DirectionVector = (TargetLocation - GetActorLocation()).GetSafeNormal();
		SetActorLocation(GetActorLocation() + DirectionVector * MoveSpeed * DeltaTime);//(MoveDirection * DeltaTime * MoveSpeed));
	}
	
	if (FVector::Dist(GetActorLocation(), TargetLocation) <= 1.f)
	{
		TargetOffset = -TargetOffset;
		TargetLocation = StartLocation + TargetOffset;
	}

}

