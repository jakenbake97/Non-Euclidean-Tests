// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NonEuclideanTestHUD.generated.h"

UCLASS()
class ANonEuclideanTestHUD : public AHUD
{
	GENERATED_BODY()

public:
	ANonEuclideanTestHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

