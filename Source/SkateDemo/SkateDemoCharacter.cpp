// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkateDemoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASkateDemoCharacter

ASkateDemoCharacter::ASkateDemoCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement 
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//Create skate skeletal mesh component
	SkateboardSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkateBoardSkeletalMesh"));
	SkateboardSkeletalMesh->SetupAttachment(GetCapsuleComponent());

	GetCharacterMovement()->BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
}

void ASkateDemoCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASkateDemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASkateDemoCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASkateDemoCharacter::Look);

		// Speed Up
		EnhancedInputComponent->BindAction(SpeedUpAction, ETriggerEvent::Triggered, this, &ASkateDemoCharacter::SpeedUp);
		EnhancedInputComponent->BindAction(SpeedUpAction, ETriggerEvent::Completed, this, &ASkateDemoCharacter::StopSpeedUp);

		// Slow Down 
		EnhancedInputComponent->BindAction(SlowDownAction, ETriggerEvent::Triggered, this, &ASkateDemoCharacter::SlowDown);
		EnhancedInputComponent->BindAction(SlowDownAction, ETriggerEvent::Completed, this, &ASkateDemoCharacter::StopSlowDown);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASkateDemoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentAbilityType)
	{
	case EAbilityType::SpeedUp:
	{
		float CurrentMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		if (CurrentMaxWalkSpeed >= SpeedUpMaxWalkSpeed)
		{
			return;
		}
		float InterpMaxWalkSpeed = FMath::Lerp(CurrentMaxWalkSpeed, SpeedUpMaxWalkSpeed, SpeedUpInterpolationAlpha);
		GetCharacterMovement()->MaxWalkSpeed = InterpMaxWalkSpeed;
		break;
	}
	case EAbilityType::SlowDown:

		break;

	default:
		break;
	}
}

void ASkateDemoCharacter::Move(const FInputActionValue& Value)
{

	if (CurrentAbilityType == EAbilityType::SlowDown)
	{
		return;
	}
	if (Controller == nullptr)
	{
		return;
	}
	// input is a Vector2D
	FVector MovementVector = Value.Get<FVector>();

	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	// add movement 
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ASkateDemoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y*0.01);
	}
}


void ASkateDemoCharacter::SpeedUp(const FInputActionValue& Value)
{
	if (CurrentAbilityType != EAbilityType::None)
	{
		return;
	}
	CurrentAbilityType = EAbilityType::SpeedUp;
}

void ASkateDemoCharacter::StopSpeedUp(const FInputActionValue& Value)
{
	if (CurrentAbilityType != EAbilityType::SpeedUp)
	{
		return;
	}
	CurrentAbilityType = EAbilityType::None;
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
}

void ASkateDemoCharacter::SlowDown(const FInputActionValue& Value)
{
	if (CurrentAbilityType != EAbilityType::None)
	{
		return;
	}
	CurrentAbilityType = EAbilityType::SlowDown;
	GetCharacterMovement()->BrakingDecelerationWalking = SlowDownBrakingDecelerationWalking;
}

void ASkateDemoCharacter::StopSlowDown(const FInputActionValue& Value)
{
	if (CurrentAbilityType != EAbilityType::SlowDown)
	{
		return;
	}
	CurrentAbilityType = EAbilityType::None;
	GetCharacterMovement()->BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
}
