// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "../GameCodeTypes.h"

#include "GCLedgeDetectorComponent.generated.h"

USTRUCT(BlueprintType)
struct FLedgeDescription
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category= "Ledge description")
	FTransform TranformWS;

	TWeakObjectPtr<UPrimitiveComponent> LedgeComponent;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZPROJECT_API UGCLedgeDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGCLedgeDetectorComponent();
	bool DetectLedge(OUT FLedgeDescription& LedgeDescription);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category= "Detection settings", meta= (UIMin=0.0f, ClampMin=0.0f))
	float MinLedgeHeight=40.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Detection settings", meta= (UIMin=0.0f, ClampMin=0.0f))
	float MaxLedgeHeight=200.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Detection settings", meta= (UIMin=0.0f, ClampMin=0.0f))
	float ForwardCheckDistance=150.0f;

private:
	TWeakObjectPtr<class ACharacter> CachedCharacterOwner;
	
	float OverlapCapsuleHalfHeight  = 0.0f;		
	
};
