// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "SkateDemoCharacter.generated.h"

UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	None UMETA(DisplayName = "None"),
	SlowDown UMETA(DisplayName = "SlowDown"),
	SpeedUp UMETA(DisplayName = "SpeedUp")
};

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAnimationAsset;
class USkeletalMeshComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ASkateDemoCharacter : public ACharacter
{
	GENERATED_BODY()


	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Skate mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skate", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkateboardSkeletalMesh;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Speed Up Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpeedUpAction;

	/** Slow Down Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlowDownAction;


public:
	ASkateDemoCharacter();

	EAbilityType CurrentAbilityType = EAbilityType::None;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SkatePlayer|SkaterAbilities")
	float DefaultBrakingDecelerationWalking = 250.0f;

	UPROPERTY(EditDefaultsOnly, Category = "SkatePlayer|SkaterAbilities")
	float SlowDownBrakingDecelerationWalking = 550.0f;

	UPROPERTY(EditDefaultsOnly, Category = "SkatePlayer|SkaterAbilities")
	float DefaultMaxWalkSpeed = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "SkatePlayer|SkaterAbilities")
	float SpeedUpInterpolationAlpha = 0.04f;

	UPROPERTY(EditDefaultsOnly, Category = "SkatePlayer|SkaterAbilities")
	float SpeedUpMaxWalkSpeed = 950.0f;

private:
virtual void Tick(float DeltaTime) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for Speed Up input*/
	void SpeedUp(const FInputActionValue& Value);
	void StopSpeedUp(const FInputActionValue& Value);

	/** Called for SlowDown input */
	void SlowDown(const FInputActionValue& Value);
	void StopSlowDown(const FInputActionValue& Value);


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = "SkatePlayer")
	FORCEINLINE USkeletalMeshComponent* GetSkateSkeletalMesh() const { return SkateboardSkeletalMesh; }

	UFUNCTION(BlueprintCallable, Category = "SkatePlayer")
	FORCEINLINE EAbilityType GetCurrentAbilityType() const { return CurrentAbilityType; }

};

