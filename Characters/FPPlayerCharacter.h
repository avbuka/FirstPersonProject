// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCPlayerCharacter.h"
#include "FPPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class XYZPROJECT_API AFPPlayerCharacter : public AGCPlayerCharacter
{
	GENERATED_BODY()
	
public:
	
	void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;	
	void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Character | First person")
	USkeletalMeshComponent* FirstPersonMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
	UCameraComponent* FirstPersonCameraComponent;

private:
};
