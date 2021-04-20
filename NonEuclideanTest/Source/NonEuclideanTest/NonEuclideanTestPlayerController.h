// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NonEuclideanTestPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NONEUCLIDEANTEST_API ANonEuclideanTestPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
public:
	
	void PerformCameraCut() const;
	FMatrix GetCameraProjectionMatrix() const;

	private:
	UPROPERTY()
	class UNonEuclideanLocalPlayer* LocalPlayer;
};
