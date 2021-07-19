// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "InteractiveCameraActor.generated.h"

/**
 * 
 */
UCLASS()
class XYZPROJECT_API AInteractiveCameraActor : public ACameraActor
{
	GENERATED_BODY()
public:
	AInteractiveCameraActor();

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float TransitionTime=2;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float TransitionBackTime=3;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) ;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UBoxComponent* BoxComponent;
	
private:
	
};
