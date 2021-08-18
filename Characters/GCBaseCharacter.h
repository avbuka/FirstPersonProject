// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GCBaseCharacter.generated.h"

class UGCBaseCharacterMovementComponent;
class UCurveVector;
class UAnimMontage;
class AInteractiveActor;

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UAnimMontage* MantleMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveVector* MantlingCurve;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin=0,UIMin=0.0f))
	float MaxHight=200.0f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin=0,UIMin=0.0f))
	float MinHight=100.0f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin=0,UIMin=0.0f))
	float MaxHightStartTime=0.0f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin=0,UIMin=0.0f))
	float MinHightStartTime=0.5f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin=0,UIMin=0.0f))
	float AnimationCorrecitonXY=65.0f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin=0,UIMin=0.0f))
	float AnimationCorrectionZ=200.0f;

};

UCLASS(Abstract,NotBlueprintable)
class XYZPROJECT_API AGCBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGCBaseCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void LookUp(float Value) {};
	
	virtual void SwimForward(float Value) {};
	virtual void SwimRight(float Value) {};
	virtual void SwimUp(float Value) {};
	
	virtual void Turn(float Value) {};
	virtual void LookUpAtRate(float Value) {};
	virtual void TurnAtRate(float Value) {};
	
	virtual void ChangeCrouchState();
	virtual void StartSprint();
	virtual void StopSprint();
	virtual void ChangeCrawlState();
	virtual void ToggleSlide();
	
	//@set true you want to forcibly initiate Mantling
	virtual void Mantle(bool bForce=false);
	virtual bool CanMantle();
	virtual bool CanJumpInternal_Implementation() const;
	virtual void Jump() override;

	virtual void ClimbLadderUp(float Value);
	virtual void InteractWithLadder();
	virtual void InteractWithZipline();
	virtual void InteractWithIActor();

	const class ALadder* GetAvailableLadder();
	const class AZipline* GetAvailableZipline();

	void TryChangeSprintState(float DeltaSeconds);
	void UpdateStamina(float DeltaSeconds);
	void RegisterInteractiveActor(AInteractiveActor* IActor);
	void UnRegisterInteractiveActor(AInteractiveActor* IActor);
	

	float GetUnchrouchedHalfHeight() const { return UnchrouchedHalfHeight; }
	void SetUnchrouchedHalfHeight(float val) { UnchrouchedHalfHeight = val; }

	UGCBaseCharacterMovementComponent* GetCharacterBaseMovementComponent() const
		{ return GCBaseCharacterMovementComponent; };

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleSlowmo();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetIKLeftFootOffset() { return IKLeftFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetIKRightFootOffset() { return IKRightFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetIKHipOffset() { return IKHipOffset; }
	
	/** Default crawled eye height */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CrawledEyeHeight=20;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Character | Movement ")
	class UGCLedgeDetectorComponent* LedgeDetector;


	const FName& GetGrabbingHandSocketName() const { return GrabbingHandSocketName; }
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category="Character | Movement | Stamina", 
		meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxStamina=100.0f;															
																				
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category="Character | Movement | Stamina", 
		meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float StaminaRestoreVelocity=2.0f;
																				
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category="Character | Movement | Stamina", 
		meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintStaminaConsumprionVelocity=5.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Character | Controls")
	float BaseTurnRate=45.0f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Character | Controls")
	float BaseLookUpRate=45.0f;
	

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintStart();
	virtual void OnSprintStart_Implementation() PURE_VIRTUAL(AGCBaseCharacter::OnSprintStart_Implementation(),);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintStop();
	virtual void OnSprintStop_Implementation()PURE_VIRTUAL(AGCBaseCharacter::OnSprintStart_Implementation(), );
	
	UFUNCTION()
	void OnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK Settings")
	FName LeftFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK Settings")
	FName RightFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK Settings",
		meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float LineTraceExtendDistance = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK Settings", 
		meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKInterpSpeed = 20.0f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Character | Mantling")
	FMantlingSettings HighMantleSettings;	
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Character | Mantling")
	FMantlingSettings LowMantleSettings;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Mantling",
		meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float LowMantleHeight = 125.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Ziplining")
	FName GrabbingHandSocketName;

	float UnchrouchedHalfHeight = 0.0f;

	
	virtual bool CanSprint() ;


	UGCBaseCharacterMovementComponent* GCBaseCharacterMovementComponent;

	TArray<AInteractiveActor*> InteractiveActors;
private:

	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;
	float GetIKFootOffset(const FName& SocketName);
	void UpdateIKSettings(float DeltaSeconds);

	
	bool bIsSprintRequested = false;
	
	float IKRightFootOffset = 0.0f;
	float IKLeftFootOffset = 0.0f;
	float IKHipOffset = 0.0f;
	float HipOffset = 0.0f;

	float IKTraceDistance = 40.0;
	float IKScale = 0.0;

	float CurrentStamina = 0.0f;

public:

};
