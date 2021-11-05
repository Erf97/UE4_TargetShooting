// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "ShotingTargetCharacter.h"
#include "GameFramework/HUD.h"
#include "ShotingTargetHUD.generated.h"

UCLASS()
class AShotingTargetHUD : public AHUD
{
	GENERATED_BODY()
	UPROPERTY()
	UFont* DefaultFont;
	
public:
	AShotingTargetHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	UFUNCTION(BlueprintCallable)
	void DrawAmmo(AShotingTargetCharacter* Player);

	void DrawTimer();

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

