// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Target.generated.h"

UCLASS()
class SHOTINGTARGET_API ATarget : public AActor
{
	GENERATED_BODY()
	// 靶子的网格体
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TargetMesh;
	// 靶子的sub mesh，只有score mesh被击中才视为有效
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ScoreMesh;
	// 靶子在场景中移动

public:
	// Sets default values for this actor's properties
	ATarget();
	// 当靶子被击中时执行的逻辑，可以在蓝图子类中重写
	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetHit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UStaticMeshComponent* GetTargetMesh() const { return TargetMesh; }
	UStaticMeshComponent* GetScoreMesh() const { return ScoreMesh; }
};
