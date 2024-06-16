// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkateDemoGameMode.h"
#include "SkateDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASkateDemoGameMode::ASkateDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/SkateDemo/Blueprints/Player/BP_SkaterCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ASkateDemoGameMode::AddPointsOnSuccessfulTrick(float PointsToAdd)
{
	TotalPlayerPoints += PointsToAdd;
	OnScoreUpdate.Broadcast(TotalPlayerPoints);
}

void ASkateDemoGameMode::RemovePointsOnFailedTrick(float ToRemove)
{
	TotalPlayerPoints = FMath::Max(TotalPlayerPoints - ToRemove, 0.0f);
	OnScoreUpdate.Broadcast(TotalPlayerPoints);
}
