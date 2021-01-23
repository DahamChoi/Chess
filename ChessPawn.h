// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ChessPawn.generated.h"

UCLASS()
class CHESS_API AChessPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AChessPawn();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	// Click Event Bind
	void TriggerClick();

	// Trace Unit
	void TraceForUnit(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

private:
	UPROPERTY()
	TArray<class AChessUnitActor*> LegalUnitArray;

	UPROPERTY()
	TArray<class AChessUnitActor*> TakeUnitArray;

	UPROPERTY()
	class AChessUnitActor* SelectedUnit;

	UPROPERTY()
	class AChessUnitActor* SelectedLegalUnit;

	UPROPERTY()
	TArray<AChessUnitActor*> CastlingRookLegalUnitArray;

	UPROPERTY()
	bool IsRunTimer;

	UPROPERTY()
	FVector LegalTargetLocation;

	UPROPERTY()
	class AChessGameManager* GameManager;
};
