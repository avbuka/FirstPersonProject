// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCBaseCharacterAniminstance.h"
#include "FPBaseCharacterAniminstance.generated.h"

/**
 * 
 */
UCLASS()
class XYZPROJECT_API UFPBaseCharacterAniminstance : public UGCBaseCharacterAniminstance
{
	GENERATED_BODY()
	
public:

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Animations | First person")
	float PlayerCamerPitchAngle=0.0f;

	TWeakObjectPtr<class AFPPlayerCharacter> ChachedFPCharacterOwner;

private:

};
