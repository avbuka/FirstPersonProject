// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCBaseCharacter.h"
#include "Components/TimelineComponent.h"

#include "GCPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class XYZPROJECT_API AGCPlayerCharacter : public AGCBaseCharacter
{
	GENERATED_BODY()
public:
	AGCPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;
	virtual void LookUpAtRate(float Value) override;
	virtual void TurnAtRate(float Value) override;

	virtual void SwimForward(float Value);
	virtual void SwimRight(float Value);
	virtual void SwimUp(float Value);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;	


	
	//recalculate base eye height and move the mesh up
	virtual void OnCrawlStart(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	///recalculate base eye height and move the mesh down
	virtual void OnCrawlEnd(float HeightAdjust, float ScaledHeightAdjust);
	virtual void RecalculateBaseEyeHeight() override;
	
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation()  override;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetInputForward() { return InputForward; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetInputRight() { return InputRight; }


	
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Character | Camera")
	class UCameraComponent* CameraComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Character | Camera")
	class USpringArmComponent* SpringArmComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Sprint |Camera")
	float SprintSpringArmTargetLength = 700.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Sprint |Camera")
	float SprintSpringTimelineDeltaTime = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character | Sprint |Camera")
	UCurveFloat* USpringArmSprintCurve;

	virtual void OnSprintStop_Implementation() ;
	virtual void OnSprintStart_Implementation() ;


	FOnTimelineFloatStatic InterpFunction;

private:

	float InputRight=0.0f;
	float InputForward=0.0f;
	float DefaultSpringArmTargetLength = 0.0f;


	FTimeline TimeLine;

	void UpdateSprintCamera(float Value);
	void BeginSprintCamera() { TimeLine.Play(); }
	void EndSprintCamera() { TimeLine.Reverse(); }

};