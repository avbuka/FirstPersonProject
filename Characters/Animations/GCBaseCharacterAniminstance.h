// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../GCBaseCharacter.h"
#include "../GCPlayerCharacter.h"
#include "../Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "GCBaseCharacterAniminstance.generated.h"

/**
 * 
 */
UCLASS()
class XYZPROJECT_API UGCBaseCharacterAniminstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	//current character speed
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Character animation")
	float CharacterSpeed=0.0f;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	float LadderSpeedRatio = 0.0f;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Character animation")
	bool bIsFalling=false;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Character animation")
	bool bIsCrouching=false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsSprinting = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsCrawling = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsOutOfStamina = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsSwimming = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsClimbing = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsZiplining = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsWallRunning = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	EWallRunningSide WallrunningSide= EWallRunningSide::None;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Character animation| IK Settings")
	FVector RightFootEffectorLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Character animation| IK Settings")
	FVector LeftFootEffectorLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Character animation| IK Settings")
	FVector IKHipOffset = FVector::ZeroVector;

private:
	TWeakObjectPtr<class AGCPlayerCharacter> CachedBasedCharacter;
	
};
