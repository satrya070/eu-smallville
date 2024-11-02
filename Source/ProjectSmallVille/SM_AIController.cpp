// Fill out your copyright notice in the Description page of Project Settings.


#include "SM_AIController.h"

void ASM_AIController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		// UE_LOG(LogTemp, Display, TEXT("Tree is running"));
	}
}
