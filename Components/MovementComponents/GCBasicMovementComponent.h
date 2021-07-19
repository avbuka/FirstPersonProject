// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GCBasicMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class XYZPROJECT_API UGCBasiMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

		virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	
	void JumpStart();
	virtual bool IsFalling() const override;
protected:

	UPROPERTY(EditAnywhere)
	float MaxSpeed=100.0f;
		
	UPROPERTY(EditAnywhere)
	float InitialJumpVelocity=500.0f;

	UPROPERTY(EditAnywhere)
	bool bEnableGravity;
private:
	FVector VerticalVelocity = FVector::ZeroVector;

	bool bIsFalling;
	
};