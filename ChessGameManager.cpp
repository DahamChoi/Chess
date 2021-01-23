// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameManager.h"
#include "Engine/World.h"
#include "State.h"
#include "ChessUnitActor.h"
#include "ChessGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AChessGameManager::AChessGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AChessGameManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), actors);
	for (AActor* actor : actors)
	{
		if (!actor->GetName().Compare("BlackCameraActor"))
		{
			BlackCamera = actor;
		}

		if (!actor->GetName().Compare("WhiteCameraActor"))
		{
			WhiteCamera = actor;
		}

		if (!actor->GetName().Compare("MainMenuCameraActor"))
		{
			MainMenuCamera = actor;
		}
	}

	this->initState();
}

// Called every frame
void AChessGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChessGameManager::ChangeViewTarget(float delay)
{
	GetWorldTimerManager().SetTimer(
		TimerHandle, this, &AChessGameManager::ChangeViewTargetTimerFunction, delay, true);
}

void AChessGameManager::ChangeViewTargetTimerFunction()
{
	State* state = GetWorld()->GetGameState<AChessGameStateBase>()->mState;
	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->SetViewTargetWithBlend(
			(state->isFirstPlayer() ? WhiteCamera : BlackCamera), 2.f,EViewTargetBlendFunction::VTBlend_Cubic);
	}
}

void AChessGameManager::NextState(int action)
{
	State* state = GetWorld()->GetGameState<AChessGameStateBase>()->mState;
	Position actionIndex = State::actionToIndex(action);
	int src = actionIndex.X;
	int dst = actionIndex.Y;

	TArray<int32>* pieces = state->mPieces;
	TArray<int32>* enemyPieces = state->mEnemyPieces;

	if (action == ACTION_KING_SIDE_CASTLING)
	{
		if (state->isFirstPlayer())
		{
			WhiteChessUnitArray[62] = WhiteChessUnitArray[60];
			WhiteChessUnitArray[61] = WhiteChessUnitArray[63];
			WhiteChessUnitArray[60] = 0;
			WhiteChessUnitArray[63] = 0;
		}
		else
		{
			BlackChessUnitArray[5] = BlackChessUnitArray[7];
			BlackChessUnitArray[6] = BlackChessUnitArray[4];
			BlackChessUnitArray[4] = 0;
			BlackChessUnitArray[7] = 0;
		}
	}
	else if (action == ACTION_QUEEN_SIDE_CASTLING)
	{
		if (state->isFirstPlayer())
		{
			WhiteChessUnitArray[58] = WhiteChessUnitArray[60];
			WhiteChessUnitArray[59] = WhiteChessUnitArray[56];
			WhiteChessUnitArray[60] = 0;
			WhiteChessUnitArray[56] = 0;
		}
		else
		{
			BlackChessUnitArray[2] = BlackChessUnitArray[4];
			BlackChessUnitArray[3] = BlackChessUnitArray[0];
			BlackChessUnitArray[0] = 0;
			BlackChessUnitArray[4] = 0;
		}
	}
	else
	{
		// 앙파상
		if (FMath::Abs<int>(dst - src) != 8 &&
			FMath::Abs<int>(dst - src) != 16 &&
			(*pieces)[src] == 6 && (*enemyPieces)[dst] == 0)
		{
			int dstX = dst % 8;
			int dstY = dst / 8;
			int takePos = ((dstY + (state->isFirstPlayer() ? 8 : -8)) * 8) + dstX;

			if (state->isFirstPlayer())
			{
				WhiteChessUnitArray[dst] = WhiteChessUnitArray[src];
				WhiteChessUnitArray[src] = 0;
				BlackChessUnitArray[takePos] = 0;
			}
		}
		else
		{
			if (state->isFirstPlayer())
			{
				WhiteChessUnitArray[dst] = WhiteChessUnitArray[src];
				WhiteChessUnitArray[src] = 0;
				BlackChessUnitArray[src] = 0;
			}
			else
			{
				BlackChessUnitArray[dst] = BlackChessUnitArray[src];
				BlackChessUnitArray[src] = 0;
				WhiteChessUnitArray[src] = 0;
			}
		}	
	}

	state->next(action);
}

void AChessGameManager::MoveTakeUnitToDeadSpace(int index)
{
	State* state = GetWorld()->GetGameState<AChessGameStateBase>()->mState;
	AChessUnitActor * unit = GetChessUnitByIndex(!state->isFirstPlayer(), index);

	if (state->isFirstPlayer())
	{
		unit->LerpMoveToTarget(FVector(
			ZERO_TAKE_UNIT_WHITE_POS_X + (WhiteTakeUnitCount++ * ZERO_TAKE_UNIT_SPACE_WHITE_POS_X),
			ZERO_TAKE_UNIT_WHITE_POS_Y,
			DEFAULT_TAKE_UNIT_POS_Z));
	}
	else
	{
		unit->LerpMoveToTarget(FVector(
			ZERO_TAKE_UNIT_BLACK_POS_X + (BlackTakeUnitCount++ * ZERO_TAKE_UNIT_SPACE_BLACK_POS_X),
			ZERO_TAKE_UNIT_BLACK_POS_Y,
			DEFAULT_TAKE_UNIT_POS_Z));
	}
}

void AChessGameManager::initState()
{
	AChessGameStateBase* gameState = GetWorld()->GetGameState<AChessGameStateBase>();
	State* state = gameState->mState;
	state->initGameBoard();

	TArray<int32>* pieces = state->mPieces;
	TArray<int32>* enemyPieces = state->mEnemyPieces;

	for (int i = 0; i < pieces->Num(); i++)
	{
		int x = i % 8;
		int y = i / 8;

		if ((*pieces)[i] != 0)
		{
			AChessUnitActor* unit = GetWorld()->SpawnActor<AChessUnitActor>(
				this->PositionToVector(x, y), FRotator::ZeroRotator);
			unit->InitializeActor((EUnitEnum)(*pieces)[i], true, false);
			WhiteChessUnitArray[i] = unit;
		}
		else
			WhiteChessUnitArray[i] = 0;
	}

	for (int i = 0; i < enemyPieces->Num(); i++)
	{
		int x = i % 8;
		int y = i / 8;

		if ((*enemyPieces)[i] != 0)
		{
			AChessUnitActor* unit = GetWorld()->SpawnActor<AChessUnitActor>(
				this->PositionToVector(x, y), FRotator(0.f, 180.f, 0.f));
			unit->InitializeActor((EUnitEnum)(*enemyPieces)[i], false, false);
			BlackChessUnitArray[i] = unit;
		}
		else
			BlackChessUnitArray[i] = 0;
	}
}

AChessUnitActor* AChessGameManager::GetChessUnitByIndex(bool isWhite, int index)
{
	return isWhite ? WhiteChessUnitArray[index] : BlackChessUnitArray[index];
}

void AChessGameManager::ChangeGameState()
{
	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

	// 게임화면으로
	if (!IsPlayGame)
	{
		if (PC)
		{
			PC->SetViewTargetWithBlend(WhiteCamera, 2.f, EViewTargetBlendFunction::VTBlend_Cubic);
		}
	}
	// 메인메뉴로
	else
	{
		PC->SetViewTargetWithBlend(MainMenuCamera, 2.f, EViewTargetBlendFunction::VTBlend_Cubic);
		
		for (int i = 0; i < 64; i++)
		{
			if (WhiteChessUnitArray[i])
			{
				WhiteChessUnitArray[i]->Destroy();
				WhiteChessUnitArray[i] = 0;
			}

			if (BlackChessUnitArray[i])
			{
				BlackChessUnitArray[i]->Destroy();
				BlackChessUnitArray[i] = 0;
			}
		}

		this->initState();
	}

	this->IsPlayGame = !this->IsPlayGame;
}

void AChessGameManager::CheckMate()
{
	OnCheckMateDelegate.Broadcast();
}
