// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCodeBasePawn.h"
#include "SpiderPawn.generated.h"

/**
 * 
 */
UCLASS()
class XYZPROJECT_API ASpiderPawn : public AGameCodeBasePawn
{
	GENERATED_BODY()

public:
	ASpiderPawn();
		
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable,BlueprintPure)
	float GetIKRightFrontFoorOffset() const {return IKRightFrontFootOffset;}
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	float GetIKRightRearFoorOffset() const { return   FMath::RoundHalfToEven( IKRightRearFootOffset);}
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	float GetIKLeftFrontFoorOffset() const {return IKLeftFrontFootOffset;}
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	float GetIKLeftRearFoorOffset() const {return IKLeftRearFootOffset;}

	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spider bot")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Spider bot| IK Settings")
	FName RightFrontFootSocketName;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Spider bot| IK Settings")
	FName RightRearFootSocketName;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Spider bot| IK Settings")
	FName LeftFrontFootSocketName;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Spider bot| IK Settings")
	FName LeftRearFootSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spider Bot| IK Settings", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKTraceExtendDistance = 30.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Spider Bot| IK Settings", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKInterpSpeed = 20.0f;

private:
	float IKRightFrontFootOffset=0.0f;
	float IKRightRearFootOffset=0.0f;
	float IKLeftRearFootOffset=0.0f;
	float IKLeftFrontFootOffset=0.0f;

	float IKTraceDistance = 0.0f;
	float IKScale = 0.0f;

	float GetIKOffsetForASocket(const FName& SocketName);
};
