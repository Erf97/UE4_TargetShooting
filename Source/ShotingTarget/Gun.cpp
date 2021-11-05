// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "ShotingTargetCharacter.h"
#include "ShotingTargetProjectile.h"

// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create a gun mesh component
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetOnlyOwnerSee(false); // otherwise won't be visible in the multiplayer
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	GunMesh->SetupAttachment(RootComponent);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(GunMesh);
	MuzzleLocation->SetRelativeLocation(RelativeLocationVector);
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGun::Fire()
{
	GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, "Fire!");
	if (ProjectileClass != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, ProjectileClass->GetClass()->GetName());
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, "World!");
			if (CurrAmmo)
			{
				ConsumeAmmo();
				const FRotator SpawnRotation = GunOwner->GetControlRotation();
				const FVector SpawnLocation = ((MuzzleLocation != nullptr)
					                               ? MuzzleLocation->GetComponentLocation()
					                               : GetActorLocation()) + SpawnRotation.RotateVector(GunOwner->GunOffset);
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride =
					ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				World->SpawnActor<AShotingTargetProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, "No Ammo!");
			}
		}
	}
}

void AGun::ConsumeAmmo()
{
	CurrAmmo --;
}

void AGun::SetParams()
{
}

int AGun::GetCurrAmmo()
{
	return CurrAmmo;
}

int AGun::GetMaxAmmo()
{
	return MaxAmmo;
}


void AGun::SetOwner(AShotingTargetCharacter* Chara)
{
	GunOwner = Chara;
}

USkeletalMeshComponent* AGun::GetGunMesh()
{
	return GunMesh;
}
