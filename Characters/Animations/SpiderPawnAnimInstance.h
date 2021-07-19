// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCBasePawnAnimInstance.h"
#include "SpiderPawnAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class XYZPROJECT_API USpiderPawnAnimInstance : public UGCBasePawnAnimInstance
{
	GENERATED_BODY()
	
public:	 
	
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
protected:
	UPROPERTY(VisibleAnywhere,Transient,BlueprintReadOnly,Category="Spider Bot| IK Settings")
	FVector RightFrontFootEffectorLocation=FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere,Transient,BlueprintReadOnly,Category="Spider Bot| IK Settings")
	FVector RightRearFootEffectorLocation=FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere,Transient,BlueprintReadOnly,Category="Spider Bot| IK Settings")
	FVector LeftFrontFootEffectorLocation=FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere,Transient,BlueprintReadOnly,Category="Spider Bot| IK Settings")
	FVector LeftRearFootEffectorLocation=FVector::ZeroVector;



private:
	TWeakObjectPtr<class ASpiderPawn> CachedSpiderPawnOwner;
};
