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
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASkateDemoCharacter::Move(const FInputActionValue& Value)
{
	if (Controller == nullptr)
	{
		return;
	}
	// input is a Vector2D
	FVector MovementVector = Value.Get<FVector>();
	/*MovementVector.X = FMath::Clamp(MovementVector.X, -0.5, 0.5);*/
	/*MovementVector.X = FMath::Clamp(MovementVector.X, -0.5, 0.5);*/
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


void ASkateDemoCharacter::Jump()
{
	Super::Jump();
	if (SkateboardSkeletalMesh == nullptr || SkateAnimationJump == nullptr)
	{
		return;
	}
	SkateboardSkeletalMesh->PlayAnimation(SkateAnimationJump, false);
}

//void ASkateDemoCharacter::SetSkateboardAttachment(USceneComponent* AttachmentComponent, bool bShouldAttach)
//{
//	if (SkateboardSkeletalMesh == nullptr)
//	{
//		return;
//	}
//	bool bIsAttachedToInputComponent = SkateboardSkeletalMesh->IsAttachedTo(AttachmentComponent);
//	if (not bShouldAttach)
//	{
//		if (not bIsAttachedToInputComponent && IsValid(AttachmentComponent))
//		{
//			return;
//		} 
//		SkateboardSkeletalMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
//		return;
//	}
//	else
//	{
//		if (bIsAttachedToInputComponent)
//		{
//			return;
//		}
//		SkateboardSkeletalMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "rootSocket");
//	}
//
//}
