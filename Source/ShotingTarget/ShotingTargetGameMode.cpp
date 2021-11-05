// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShotingTargetGameMode.h"
#include "ShotingTargetHUD.h"
#include "ShotingTargetCharacter.h"
#include "UObject/ConstructorHelpers.h"

AShotingTargetGameMode::AShotingTargetGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AShotingTargetHUD::StaticClass();
	
}


