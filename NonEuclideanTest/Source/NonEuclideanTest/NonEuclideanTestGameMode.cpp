// Copyright Epic Games, Inc. All Rights Reserved.

#include "NonEuclideanTestGameMode.h"
#include "NonEuclideanTestHUD.h"
#include "NonEuclideanTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANonEuclideanTestGameMode::ANonEuclideanTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ANonEuclideanTestHUD::StaticClass();
}
