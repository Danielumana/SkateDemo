// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseObstacle.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "../SkateDemoCharacter.h"
#include "../SkateDemoGameMode.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseObstacle::ABaseObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ObstacleCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ObstacleCollision"));
	ObstacleCollision->SetupAttachment(RootComponent);
	FVector ObstacleCollisionExtents(55.9f, 89.4f, 55.0f);
	ObstacleCollision->SetBoxExtent(ObstacleCollisionExtents);

	TrickValidationZone = CreateDefaultSubobject<USphereComponent>(TEXT("TrickValidationZone"));
	TrickValidationZone->SetupAttachment(ObstacleCollision);
	TrickValidationZone->SetSphereRadius(170.0f);

	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
	ObstacleMesh->SetupAttachment(ObstacleCollision);

	UpdatePlayerReference();
}

// Called when the game starts or when spawned
void ABaseObstacle::BeginPlay()
{
	Super::BeginPlay();
	UWorld* WorldObjectReference = GetWorld();

	if (WorldObjectReference == nullptr)
	{
		return;
	}

	GameModeReference = Cast<ASkateDemoGameMode>(UGameplayStatics::GetGameMode(WorldObjectReference));
	UE_LOG(LogTemp, Warning, TEXT("gamemode"), UGameplayStatics::GetGameMode(WorldObjectReference));
}

void ABaseObstacle::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	TrickValidationZone->OnComponentBeginOverlap.AddDynamic(this, &ABaseObstacle::OnOverlapBegin);
	TrickValidationZone->OnComponentEndOverlap.AddDynamic(this, &ABaseObstacle::OnOverlapEnd);
}

void ABaseObstacle::UpdatePlayerReference()
{
	UWorld* WorldObjectReference = GetWorld();

	if (WorldObjectReference == nullptr)
	{
		return;
	}
	
	APlayerController* PlayerController = WorldObjectReference->GetFirstPlayerController();
	if (PlayerController == nullptr)
	{
		return;
	}
	PlayerReference = PlayerController->GetCharacter();
}

// Called every frame
void ABaseObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseObstacle::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsPlayerOnValidTrick)
	{
		return;
	}
	ASkateDemoCharacter* OverlapCharacter = Cast<ASkateDemoCharacter>(OtherActor);
	if (OverlapCharacter == nullptr)
	{
		return;
	}
	UPawnMovementComponent* MovementComponent = OverlapCharacter->GetMovementComponent();
	if (MovementComponent == nullptr)
	{
		return;
	}
	bool bIsFalling = MovementComponent->IsFalling();
	if (not bIsFalling)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Overlap start"));
	bIsPlayerOnValidTrick = true;
}

void ABaseObstacle::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASkateDemoCharacter* OverlapCharacter = Cast<ASkateDemoCharacter>(OtherActor);
	if (OverlapCharacter == nullptr)
	{
		return;
	}
	if (bIsPlayerOnValidTrick == false)
	{
		return;
	}
	if (GameModeReference == nullptr)
	{
		return;
	}
	UPawnMovementComponent* MovementComponent = OverlapCharacter->GetMovementComponent();
	if (MovementComponent == nullptr)
	{
		return;
	}
	bool bIsFalling = MovementComponent->IsFalling();
	if (not bIsFalling)
	{
		bIsPlayerOnValidTrick = false;
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Overlap end"));
	GameModeReference->AddPointsOnSuccessfulTrick(TrickPoints);
	bIsPlayerOnValidTrick = false;
}

