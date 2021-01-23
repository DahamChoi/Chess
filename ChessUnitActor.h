// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "ChessUnitActor.generated.h"

UENUM(BlueprintType)
enum class EUnitEnum : uint8
{
	UE_EMPTY = 0 UMETA(DisplayName = "EMPTY"),
	UE_KING UMETA(DisplayName = "KING"),
	UE_QUEEN UMETA(DisplayName = "QUEEN"),
	UE_BISHOP UMETA(DisplayName = "BISHOP"),
	UE_KNIGHT UMETA(DisplayName = "KNIGHT"),
	UE_ROOK UMETA(DisplayName = "ROOK"),
	UE_PAWN UMETA(DisplayName = "PAWN"),
};

UCLASS()
class CHESS_API AChessUnitActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChessUnitActor();

	void InitializeActor(EUnitEnum type, bool white, bool opacity);

	void LerpMoveToTarget(FVector target);

	void LerpMoveToTargetReverse();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere)
	EUnitEnum UnitType;

	UPROPERTY(VisibleAnywhere)
	bool isWhite;

	UPROPERTY(VisibleAnywhere)
	bool isOpacity;

private:
	UFUNCTION()
	void HandleProgress(float value);

	UPROPERTY()
	FVector ActorInitialLocation;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	FTimeline MyTimeline;

	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* UnitStaticMesh;
	
	UPROPERTY()
	UStaticMesh* ChessKingStaticMesh;

	UPROPERTY()
	UStaticMesh* ChessQueenStaticMesh;

	UPROPERTY()
	UStaticMesh* ChessBishopStaticMesh;

	UPROPERTY()
	UStaticMesh* ChessKnightStaticMesh;

	UPROPERTY()
	UStaticMesh* ChessRookStaticMesh;

	UPROPERTY()
	UStaticMesh* ChessPawnStaticMesh;

	UPROPERTY()
	UMaterialInstance* ChessBlackMaterial;

	UPROPERTY()
	UMaterialInstance* ChessBlackOpacityMaterial;

	UPROPERTY()
	UMaterialInstance* ChessWhiteMaterial;

	UPROPERTY()
	UMaterialInstance* ChessWhiteOpacityMaterial;

	UPROPERTY()
	UCurveFloat* CurveFloat;
};
