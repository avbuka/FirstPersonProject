// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../Characters/GCPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "../Characters/GCPlayerCharacter.h"
#include "GCLedgeDetectorComponent.h"
#include "../Actors/Interactive/Environment/Ladder.h"
#include "../Actors/Interactive/Environment/Zipline.h"



#include "GCBaseCharacterMovementComponent.generated.h"

struct FMantlingMovementParameters
{
	FVector InitialLocation  = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;
	
	FTransform TargetTranformLS;

	FVector InitialAnimationLocation = FVector::ZeroVector;

	float Duration  = 0.0f;
	float StartTime = 0.0f;

	UCurveVector* MantlingCurve;

	TWeakObjectPtr<UPrimitiveComponent> LedgeComponent;

};

UENUM(BlueprintType)
enum class ECustomMovementMode :uint8
{
	CMOVE_None =0		 UMETA(DisplayName = "None"),
	CMOVE_Mantling		 UMETA(DisplayName = "Mantling"),
	CMOVE_ClimbingLadder UMETA(DisplayName = "Climbing"),
	CMOVE_Ziplining		 UMETA(DisplayName = "Ziplining"),
	CMOVE_WallRunning	 UMETA(DisplayName = "WallRunning"),
	CMOVE_Sliding		 UMETA(DisplayName = "Sliding"),
	CMOVE_Max			 UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EGCDetachMethod : uint8
{
	Fall = 0,
	ReachingTheTop,
	ReachingTheBottom,
	Jump
};

UENUM(BlueprintType)
enum class EWallRunningSide : uint8
{
	None =0,
	Right,
	Left, 
};
UCLASS()
class XYZPROJECT_API UGCBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
		
	UGCBaseCharacterMovementComponent();
	virtual void BeginPlay() override;

	bool CrawlOverlapsWithSomething();
		
	bool IsSprinting()    const { return bIsSprinting;	  }
	bool IsOutOfStamina() const { return bIsOutOfStamina; }
	bool IsCrawling()	  const	{ return bIsCrawling;     }
	bool CanEverCrawl()	  const	{ return bCanCrawl;	      }
	bool IsZiplining()	  const { return bIsZiplining;	  }
	bool IsWallRunning()  const { return bIsWallRunning;  }
	bool IsSliding()	  const { return bIsSliding;	  }
	bool IsMantling()     const;
	bool IsOnLadder()	  const;

	EWallRunningSide GetWallRunningSide() const { return CurrentWallRunningSide; }

	/** Returns true if the character is allowed to crawl in the current state.*/
	bool CanCrawlInCurrentState();
	bool CanMantleInCurrentState();
	
	virtual float GetMaxSpeed() const override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds)  override;
	virtual void PhysicsRotation(float DeltaTime) override;
	virtual void Crawl();
	virtual void UnCrawl();

	void SetIsOutOfStamina(bool bIsOutOfStamina_In);
	void SetIsCrawling(bool bIsProning_In);

	void StartSprint();
	void StopSprint();

	bool CanSlide() const;
	void StartSlide();
	void TryToEndSlide();
	void EndSlide();

	void CheckForWallRunning(UPrimitiveComponent* Comp,const FHitResult& Hit);
	void StartWallRunning(const FVector& WallNormal);
	void EndWallRunning(EGCDetachMethod Method=EGCDetachMethod::Fall);
	void WallRunningTimeOut();

	void StartMantle(const FMantlingMovementParameters& MantlingParams);
	void EndMantle();
	
	void AttachToLadder(const class ALadder* Ladder);
	void AttachToZipline(const class AZipline* Zipline);

	float GetActorToCurrentLadderProjection(const FVector& Location) const; 
	float GetLadderSpeedRatio() const;

	void DetachFromLadder(EGCDetachMethod DMethod = EGCDetachMethod::Fall);
	void DetachFromZipline();

	const ALadder* GetCurrentLadder() { return CurrentLadder; }
	const AZipline* GetCurrentZipline() { return CurrentZipline; }
	
	void CustomJumpImplementation();
	

	/** If true, try to crawl (or keep crawling) on next update. If false, try to stop crawling on next update. */
	UPROPERTY(Category = "Character Movement | Crawl", VisibleInstanceOnly, BlueprintReadOnly)
	bool bWantsToCrawl=false;
	
	/** Trying to stand up from any pose*/
	UPROPERTY(Category = "Character Movement | Crawl", VisibleInstanceOnly, BlueprintReadOnly)
	bool bWantsToStandUp=false;

	/* Keep trying to mantle*/
	UPROPERTY(Category = "Character Movement | Mantling", VisibleInstanceOnly, BlueprintReadOnly)
	bool bWantsToMantle=false;

	/* Keep trying to end sliding*/
	UPROPERTY(Category = "Character Movement | Sliding", VisibleInstanceOnly, BlueprintReadOnly)
	bool bWantsToEndSlide=false;

	/**
	 * If true, crawling should keep the base of the capsule in place by lowering the center of the shrunken capsule. If false, the base of the capsule moves up and the center stays in place.
	 * The same behavior applies when the character uncrawles: if true, the base is kept in the same location and the center moves up. If false, the capsule grows and only moves up if the base impacts something.
	 * By default this variable is set when the movement mode changes: set to true when walking and false otherwise. Feel free to override the behavior when the movement mode changes.
	 */
	UPROPERTY(Category = "Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadWrite, AdvancedDisplay)
	bool bCrawlMaintainsBaseLocation=true;

	TWeakObjectPtr<class AGCPlayerCharacter> GCPlayerCharacter;

	

protected:
	/** Called after MovementMode has changed. Base implementation does special handling for starting certain modes, then notifies the CharacterOwner. */
	
	bool CanStandUp();
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode);
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	// Custom physics functions 
	void PhysMantling(float DeltaTime, int32 Iterations);
	void PhysClimbing(float DeltaTime, int32 Iterations);
	void PhysZiplining(float DeltaTime, int32 Iterations);
	void PhysWallRunning(float DeltaTime, int32 Iterations);
	void PhysSliding(float DeltaTime, int32 Iterations);
	// Custom physics functions 

	bool AreWallRunningKeysPressed(const EWallRunningSide& CurrentSide) const;
	FVector GetWallDirection(const FVector& WallNormal, const EWallRunningSide& CurrentSide) const;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Swimming", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SwimmingCapsuleHeight = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Swimming", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SwimmingCapsuleRadius = 40.0f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category= "Character movement| Sprint",
				meta= (ClampMin=0.0f, UIMin=0.0f))
	float SprintSpeed=1200.0f;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category= "Character movement| Sprint", 
				meta= (ClampMin=0.0f, UIMin=0.0f))
	float OutOfStaminaMaxSpeed=200.0f;;

	//some generic comment about prone speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement| Crawl", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float CrawlSpeed = 100.0f;

	//can the character crawl?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character movement| Crawl")
	bool bCanCrawl = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character movement | Crawl")
	float CrawlingHalfHeight = 30;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Climbing",
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float ClimbingMaxSpeed = 200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Climbing", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float ClimbingDeceleration = 2048.0f;;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Climbing", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float LadderToCharacterOffset = 60.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Climbing", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinClimbBottomOffset = 60.0f;;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Climbing", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxClimbTopOffset = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Climbing", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float JumpOffSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Ziplining", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float ZipliningSpeed = 800.0f;
	
	// How fast we want to attach the arm to the zipline
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Ziplining", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float CharacterToZiplineMoveSpeed = 10.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Wallrunning", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float WallRunningMaxSpeed = 500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Sliding", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SlidingMaxSpeed = 500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Sliding", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxSlidingUpAngle = .0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Sliding", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	bool bCanSlide = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Sliding", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SlidingTime = 3.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Sliding", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SlidingCapsuleHalfHeight = 40.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Wallrunning", 
				meta = (ClampMin = 0.0f, UIMin = 0.0f))
	uint8 MaxNumberOfRunnableWalls = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement | Wallrunning")
	bool bIsWallrunningEnabled = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement | Wallrunning")
	UCurveFloat* WallRunningCurve;


private:
	bool bIsSprinting=false;
	bool bIsOutOfStamina = false;
	bool bIsCrawling	 = false;
	bool bIsZiplining	 = false;
	bool bIsWallRunning	 = false;
	bool bIsSliding		 = false;
	
	float TargetMantlingTime = 1.0f;

	uint8 CurrentWallNumber = 0;
	EWallRunningSide CurrentWallRunningSide = EWallRunningSide::None;

	const ALadder* CurrentLadder = nullptr;
	const AZipline* CurrentZipline = nullptr;

	FMantlingMovementParameters CurrentMantlingParameters;
	
	FTimerHandle MantlingTimer;
	FTimerHandle WallRunningTimer;
	FTimerHandle SlidingTimer;
	
	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation;
};
