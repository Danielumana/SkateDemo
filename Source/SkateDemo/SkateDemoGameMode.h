// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SkateDemoGameMode.generated.h"

UCLASS(minimalapi)
class ASkateDemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASkateDemoGameMode();

protected:

	float TotalPlayerPoints = 0.0f;

public:
	UFUNCTION()
	void AddPointsOnSuccessfulTrick(float PointsToAdd);
	
	UFUNCTION()
	void RemovePointsOnFailedTrick(float ToRemove);

	UFUNCTION(BlueprintCallable, Category = "PointsData")
	FORCEINLINE float GetTotalPoints() const { return TotalPlayerPoints; }


};



