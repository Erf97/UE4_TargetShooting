// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShotingTargetHUD.h"

// #include <gameux.h>

#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AShotingTargetHUD::AShotingTargetHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(
		TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
	ConstructorHelpers::FObjectFinder<UFont> FontObject(TEXT("/Game/Fonts/UNIDREAMLED_Font"));
	if (FontObject.Object)
	{
		DefaultFont = FontObject.Object;
	}
}


void AShotingTargetHUD::DrawHUD()
{
	Super::DrawHUD();
	// GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, "Draw HUD!");
	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X),
	                                      (Center.Y + 0.0f));

	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

	AShotingTargetCharacter* Player = Cast<AShotingTargetCharacter>(
		UGameplayStatics::GetPlayerCharacter(this, 0));

	DrawAmmo(Player);
	
}

void AShotingTargetHUD::DrawAmmo(AShotingTargetCharacter* Player)
{
	if (Player->bIsHaveWeapon)
	{
		AGun* Gun = Player->Gun;
		int CurrAmmo = Gun->GetCurrAmmo();
		int MaxAmmo = Gun->GetMaxAmmo();
		FString AmmoInfo = FString::FromInt(CurrAmmo) + FString(" / ") + FString::FromInt(MaxAmmo);
		const FVector2D InPos(Canvas->ClipX * 0.05f, Canvas->ClipY * 0.85f);
		FCanvasTextItem TextItem(InPos, FText::FromString(AmmoInfo), DefaultFont, FColor::Orange);
		Canvas->DrawItem(TextItem);
	}
}

