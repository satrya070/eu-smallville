// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectSmallVilleGameMode.h"
#include "ProjectSmallVilleCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectSmallVilleGameMode::AProjectSmallVilleGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
