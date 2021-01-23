// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChessGameManager.generated.h"

static const float ZERO_UNIT_POS_X = 1332.5f;
static const float ZERO_UNIT_POS_Y = -892.5f;
static const float DEFAULT_UNIT_POS_Z = 158.5f;
static const float SPACE_UNIT_POS_X = -15.0f;
static const float SPACE_UNIT_POS_Y = 15.0f;

static const float ZERO_TAKE_UNIT_WHITE_POS_X = 1227.f;
static const float ZERO_TAKE_UNIT_WHITE_POS_Y = -930.f;
static const float ZERO_TAKE_UNIT_BLACK_POS_X = 1330.f;
static const float ZERO_TAKE_UNIT_BLACK_POS_Y = -750.f;
static const float DEFAULT_TAKE_UNIT_POS_Z = 151.f;
static const float ZERO_TAKE_UNIT_SPACE_WHITE_POS_X = 10.f;
static const float ZERO_TAKE_UNIT_SPACE_BLACK_POS_X = -10.f;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCheckMateDelegate);

UCLASS()
class CHESS_API AChessGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChessGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static int VectorToIndex(FVector vector)
	{
		int y = (vector.X - ZERO_UNIT_POS_X) / SPACE_UNIT_POS_X;
		int x = (vector.Y - ZERO_UNIT_POS_Y) / SPACE_UNIT_POS_Y;

		return (y * 8) + x;
	}

	static FVector PositionToVector(int x, int y)
	{
		float xPos = ZERO_UNIT_POS_X + (y * SPACE_UNIT_POS_X);
		float yPos = ZERO_UNIT_POS_Y + (x * SPACE_UNIT_POS_Y);

		return FVector(xPos, yPos, DEFAULT_UNIT_POS_Z);
	}

	static FVector IndexToVector(int index)
	{
		int x = index % 8;
		int y = index / 8;

		return PositionToVector(x, y);
	}

	void ChangeViewTarget(float delay);

	void ChangeViewTargetTimerFunction();

	void NextState(int action);

	void MoveTakeUnitToDeadSpace(int index);

	void initState();

	class AChessUnitActor* GetChessUnitByIndex(bool isWhite, int index);

	UFUNCTION(BlueprintCallable)
	void ChangeGameState();

	UFUNCTION(BlueprintCallable)
	void CheckMate();

	UPROPERTY(BlueprintAssignable)
	FCheckMateDelegate OnCheckMateDelegate;

private:
	AActor* WhiteCamera;
	AActor* BlackCamera;
	AActor* MainMenuCamera;

	FTimerHandle TimerHandle;

	AChessUnitActor* WhiteChessUnitArray[64];
	AChessUnitActor* BlackChessUnitArray[64];

	int WhiteTakeUnitCount;
	int BlackTakeUnitCount;

public:
	UPROPERTY(BlueprintReadWrite)
	bool IsPlayGame;
};
