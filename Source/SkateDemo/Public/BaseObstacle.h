// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseObstacle.generated.h"


class UBoxComponent;
class USphereComponent;
class ACharacter;
class ASkateDemoGameMode;

UCLASS()
class SKATEDEMO_API ABaseObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseObstacle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

protected:
	void UpdatePlayerReference();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	
	ACharacter* PlayerReference;

	ASkateDemoGameMode* GameModeReference;
	
	bool bIsPlayerOnValidTrick = false;

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = "ObstacleComponents")
	UBoxComponent* ObstacleCollision;

	UPROPERTY(VisibleDefaultsOnly, Category = "ObstacleComponents")
	USphereComponent* TrickValidationZone;

	UPROPERTY(VisibleDefaultsOnly, Category = "ObstacleComponents")
	UStaticMeshComponent* ObstacleMesh;

	UPROPERTY(EditDefaultsOnly, Category = "ObstacleSettings")
	float TrickPoints = 10.0f;

public:
	UFUNCTION()
    void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
    void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	FORCEINLINE bool GetIsPlayerOnValidTrick() const { return bIsPlayerOnValidTrick; }

};
