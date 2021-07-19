// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameCodeBasePawn.generated.h"

UCLASS()
class XYZPROJECT_API AGameCodeBasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGameCodeBasePawn();

	UPROPERTY(VisibleAnywhere)
	UPawnMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Jump();

	UFUNCTION(BlueprintCallable,BlueprintPure)
		float GetInputForward() {return InputForward;}
	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetInputRight() {return InputRight;}

	UFUNCTION()
		void OnBlendCompleted();
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Base pawn")
	float CollisionSphereRadius=50.0f;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Base pawn")
	class USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Base pawn")
	class UCameraComponent* CameraComponent;
	
#if WITH_EDITORONLY_DATA

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Base pawn")
	class UArrowComponent* ArrowComponent;
#endif
	virtual void BeginPlay() override;

	private:
	
		float InputForward;
		float InputRight;
		

		AActor* CurrentActor;


	
};
