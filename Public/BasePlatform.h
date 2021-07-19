// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "PlatformInvocator.h"
#include "BasePlatform.generated.h"

UENUM(BlueprintType)
enum class EPlatformMovementType : uint8
{
	OnDemand = 0,   //move only on demand
	Loop            //move in an infinite cycle
};

UCLASS()
class XYZPROJECT_API ABasePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	FVector StartLocation;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
	FVector EndLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* TimelineCurve;
	
	FTimeline PlatformTimeline;

	UFUNCTION(BlueprintCallable)
	void PlatformTimelineUpdate(float Alpha);
	
	UFUNCTION(BlueprintCallable)
	void PlatformTimelineEnded(float Alpha);	

	UFUNCTION(BlueprintCallable)
	void ReversePlatformMovementDirection();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlatformMovementType PlatformBehavior = EPlatformMovementType::OnDemand;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	APlatformInvocator* PlatformInvocator;

	void OnPlatformInvoked();
};
