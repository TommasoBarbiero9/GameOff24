// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameOff24GameMode.h"
#include "GameOff24Character.h"
#include "UObject/ConstructorHelpers.h"

AGameOff24GameMode::AGameOff24GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
