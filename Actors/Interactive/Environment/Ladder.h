// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../InteractiveActor.h"
#include "Ladder.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UAnimMontage;
/**
 * 
 */
UCLASS(Blueprintable)
class XYZPROJECT_API ALadder : public AInteractiveActor
{
	GENERATED_BODY()
	
public:
	ALadder();
	float GetLadderHeight() const { return LadderHeight; }
	void SetLadderHeight(float val) { LadderHeight = val; }
	
	virtual void BeginPlay() override;


	bool GetIsCharacterOnTop() const { return bIsCharacterOnTop; }
	void SetIsCharacterOnTop(bool val) { bIsCharacterOnTop = val; }

	void SetAttachFromTopAnimMontage(class UAnimMontage* val) { AttachFromTopAnimMontage = val; }
	UAnimMontage* GetAttachFromTopAnimMontage() const { return AttachFromTopAnimMontage; }


	FVector GetAttachFromTopAnimMontageInitialOffset() const;
	void SetAttachFromTopAnimMontageInitialOffset(FVector val) { AttachFromTopAnimMontageInitialOffset = val; }
protected:

	virtual void SetObjectType() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UBoxComponent* GetLadderInteractionBox() const;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float LadderHeight = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float LadderWidth = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float StepsInterval= 25.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float BottomStepOffset = 25.0f; 
	
	//Offset from ladder's top for starting the anim montage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	FVector AttachFromTopAnimMontageInitialOffset= FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	UAnimMontage* AttachFromTopAnimMontage;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* RightRailMeshComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* LeftRailMeshComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UInstancedStaticMeshComponent* StepsMeshComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UBoxComponent* TopInteractionVolume;


private:
	bool bIsCharacterOnTop =false;

};
