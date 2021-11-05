// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class AShotingTargetCharacter;
UCLASS()
class SHOTINGTARGET_API AGun : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int MaxAmmo;

	UPROPERTY(EditAnywhere)
	int CurrAmmo;

	UPROPERTY(EditAnywhere)
	float FiringRate;

	UPROPERTY(EditAnywhere)
	float ProjectileSpeed;

	/** Projectile class to spawn */
	UPROPERTY(EditAnywhere, Category=Projectile)
	TSubclassOf<class AShotingTargetProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	FVector RelativeLocationVector;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* GunMesh;


	/** 子弹发射位置 */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly)
	AShotingTargetCharacter* GunOwner;

public:
	// Sets default values for this actor's properties
	AGun();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Fire();
	virtual void ConsumeAmmo();
	virtual void SetParams();
	UFUNCTION(BlueprintCallable)
	int GetCurrAmmo();
	UFUNCTION(BlueprintCallable)
	int GetMaxAmmo();
	void SetOwner(AShotingTargetCharacter* Chara);
	USkeletalMeshComponent* GetGunMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
