// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkateDemoGameMode.h"
#include "SkateDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASkateDemoGameMode::ASkateDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
