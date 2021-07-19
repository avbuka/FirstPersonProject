// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../InteractiveActor.h"
#include "Components/StaticMeshComponent.h"

#include "Zipline.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class XYZPROJECT_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()

public:
	AZipline();

	//check if we are higher than the lowest pole (zipline is one way)
	bool CanAttachToZipline(const FVector& ActorLocation);

	const FVector& GetCableHighestPoint() const { return CableHighestPoint; }
	const FVector& GetCableLowestPoint() const { return CableLowestPoint; }
	const FVector& GetZiplineDownVector() const { return ZiplineDownVector; }
	const FVector& GetLowerPoleLocation() const { return LowestPoleLocation;}
	float GetCableLength() const { return CableLength; }

	float GetZiplineJumpOffThreshold() const { return ZiplineJumpOffThreshold; }
	void SetZiplineJumpOffThreshold(float val) { ZiplineJumpOffThreshold = val; }



protected:
	virtual void BeginPlay() override;
	virtual void SetObjectType() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	//call OnConstruction explicitly for in-editor use
	UFUNCTION(BlueprintCallable,CallInEditor,Category="Zipline Settings")
	void ConstructZipline();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline Settings")
	float ZiplineJumpOffThreshold = 100.0f;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* FirstPoleSMComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* SecondPoleSMComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* CableSMComponent;
	

private:
	float CableLength = 0.0f;
	
	FVector ZiplineDownVector = FVector::ZeroVector;
	FVector LowestPoleLocation= FVector::ZeroVector;
	FVector CableHighestPoint = FVector::ZeroVector;
	FVector CableLowestPoint = FVector::ZeroVector;


};
