// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveActor.generated.h"

UENUM(BlueprintType)
enum class EInteractiveObjectType :uint8
{
	Ladder = 0		 UMETA(DisplayName = "Ladder"),
	Zipline			 UMETA(DisplayName = "Zipline"),
	None
};

UCLASS(Abstract,NotBlueprintable)
class XYZPROJECT_API AInteractiveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	EInteractiveObjectType GetObjectType() const { return ObjectType; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void SetObjectType() PURE_VIRTUAL(AInteractiveActor::SetObjectType(),);

	UFUNCTION()
	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool IsOverlappingCharacterCapsule(AActor* OtherActor, UPrimitiveComponent* OtherComp);

	UFUNCTION()
	virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category= "Interaction")
	UPrimitiveComponent* InteractionVolume;

	EInteractiveObjectType ObjectType;

private:


};
