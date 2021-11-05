// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShotingTargetCharacter.h"
#include "ShotingTargetProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Algo/Rotate.h"
#include "ShotingTargetHUD.h"
#include "Components/LineBatchComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AShotingTargetCharacter

AActor* AShotingTargetCharacter::GetAimingActor()
{
	FVector CamLoc;
	FRotator CamRot;
	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	APlayerController* FPSController = GetWorld()->GetFirstPlayerController();
	if (FPSController)
	{
		ULocalPlayer* LocalPlayer = FPSController->GetLocalPlayer();
		if (LocalPlayer && LocalPlayer->ViewportClient)
		{
			FVector Dir, pint;
			if (FPSController->DeprojectMousePositionToWorld(pint, Dir))
			{
				FVector TraceStart = CamLoc;
				FVector Direction = CamRot.Vector();
				FVector TranceEnd = TraceStart + (Direction * 1000);

				FCollisionQueryParams TraceParams(FName(TEXT("Actor")), true, this);
				TraceParams.bReturnPhysicalMaterial = false;
				TraceParams.bTraceComplex = true;

				FHitResult HitResult(ForceInit);
				GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TranceEnd, ECC_Visibility, TraceParams);
				// GetWorld()->PersistentLineBatcher->DrawLine(TraceStart, TranceEnd, FLinearColor::Red,10, 1.f); 
				return HitResult.GetActor();
			}
		}
	}
	return nullptr;
}

AShotingTargetCharacter::AShotingTargetCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	Gun = CreateDefaultSubobject<AGun>(TEXT("Gun"));
	bIsHaveWeapon = false;
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// // Create a gun mesh component
	// GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	// GunMesh->SetOnlyOwnerSee(false); // otherwise won't be visible in the multiplayer
	// GunMesh->bCastDynamicShadow = false;
	// GunMesh->CastShadow = false;
	// // FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	// GunMesh->SetupAttachment(RootComponent);
}


void AShotingTargetCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	// GunMesh->AttachToComponent(GetMesh1P(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
	//                        TEXT("GripPoint"));
}

//////////////////////////////////////////////////////////////////////////
// Input

void AShotingTargetCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("GetWeapon", IE_Pressed, this, &AShotingTargetCharacter::GetWeapon);
	PlayerInputComponent->BindAction("DiscardWeapon", IE_Pressed, this, &AShotingTargetCharacter::DiscardWeapon);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShotingTargetCharacter::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AShotingTargetCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShotingTargetCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShotingTargetCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShotingTargetCharacter::LookUpAtRate);
}

void AShotingTargetCharacter::OnFire()
{
	if (!bIsHaveWeapon)
	{
		return;
	}
	// try and fire a projectile
	Gun->Fire();

	// TODO:这部分挪到Gun里
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AShotingTargetCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AShotingTargetCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AShotingTargetCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AShotingTargetCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AShotingTargetCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShotingTargetCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void AShotingTargetCharacter::GetWeapon()
{
	// UKismetSystemLibrary::PrintString(this, TEXT("Hello!"));
	AActor* AimingActor = GetAimingActor();
	if (AimingActor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, AimingActor->GetName());
		if (AimingActor->GetClass()->IsChildOf(AGun::StaticClass()))
		{
			if (bIsHaveWeapon)
			{
				DiscardWeapon();
			}
			Gun = Cast<AGun>(AimingActor);
			Gun->SetOwner(this);
			USkeletalMeshComponent* GunMesh = Gun->GetGunMesh();
			GunMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			GunMesh->SetSimulatePhysics(false);
			GunMesh->AttachToComponent(GetMesh1P(),
			                           FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
			                           TEXT("GripPoint"));

			bIsHaveWeapon = true;
		}
	}
}

void AShotingTargetCharacter::DiscardWeapon()
{
	if (bIsHaveWeapon)
	{
		USkeletalMeshComponent* GunMesh = Gun->GetGunMesh();
		Gun->SetOwner(nullptr);
		GunMesh->SetCollisionResponseToAllChannels(ECR_Block);
		GunMesh->SetSimulatePhysics(true);
		GunMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		bIsHaveWeapon = false;
	}
}

bool AShotingTargetCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AShotingTargetCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AShotingTargetCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AShotingTargetCharacter::TouchUpdate);
		return true;
	}

	return false;
}
