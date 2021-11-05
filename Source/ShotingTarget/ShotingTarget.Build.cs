// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShotingTarget : ModuleRules
{
	public ShotingTarget(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
