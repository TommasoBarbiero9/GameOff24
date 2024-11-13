// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameOff24 : ModuleRules
{
	public GameOff24(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "AIModule" });
		
		PublicDependencyModuleNames.AddRange(new[] { "GameplayAbilities", "GameplayTags", "GameplayTasks" });
	}
}
