// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformInvocator.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInvocatorActivated);


UCLASS()
class XYZPROJECT_API APlatformInvocator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlatformInvocator();

public:	

	FOnInvocatorActivated OnInvocatorActivated;

	UFUNCTION(BlueprintCallable)
	void Invoke();
};
