// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPawn.h"
#include "GameFramework/PlayerController.h"
#include "ChessUnitActor.h"
#include "Engine/World.h"
#include "State.h"
#include "ChessGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "ChessGameManager.h"

// Sets default values
AChessPawn::AChessPawn()
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AChessPawn::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> actors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChessGameManager::StaticClass(), actors);
	for (AActor* actor : actors)
	{
		GameManager = Cast<AChessGameManager>(actor);
	}
}

void AChessPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("TriggerClick", EInputEvent::IE_Pressed, this, &AChessPawn::TriggerClick);
}

void AChessPawn::TriggerClick()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FVector Start, Dir, End;
		PC->DeprojectMousePositionToWorld(Start, Dir);
		End = Start + (Dir * 8000.0f);
		TraceForUnit(Start, End, false);
	}
}

void AChessPawn::TraceForUnit(const FVector& Start, const FVector& End, bool bDrawDebugHelpers)
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, FCollisionObjectQueryParams::AllDynamicObjects);
	State* state = GetWorld()->GetGameState<AChessGameStateBase>()->mState;

	if (!GameManager->IsPlayGame && state->isCheckMate())
	{
		return;
	}

	if (HitResult.Actor.IsValid())
	{
		AChessUnitActor* hitUnit = Cast<AChessUnitActor>(HitResult.Actor.Get());
		if (hitUnit)
		{
			if (!((hitUnit->isWhite && state->isFirstPlayer()) ||
				(!hitUnit->isWhite && !state->isFirstPlayer())))
				return;

			FVector actorLocation = hitUnit->GetActorLocation();

			if (!hitUnit->isOpacity)
			{
				if (LegalUnitArray.Num() == 0)
				{
					SelectedUnit = hitUnit;

					TArray<ActionStruct>* actions = state->legalActions(
						AChessGameManager::VectorToIndex(actorLocation));
					for (int i = 0; i < actions->Num(); i++)
					{
						int dstIndex = State::actionToIndex((*actions)[i].Action).Y;
						if ((*actions)[i].ActionType == EActionEnum::AE_TAKE ||
							(*actions)[i].ActionType == EActionEnum::AE_NORMAL ||
							(*actions)[i].ActionType == EActionEnum::AE_EN_PASSANT)
						{
							AChessUnitActor* unit = GetWorld()->SpawnActor<AChessUnitActor>(
								actorLocation, FRotator::ZeroRotator);
							unit->InitializeActor(hitUnit->UnitType, hitUnit->isWhite, true);
							unit->LerpMoveToTarget(AChessGameManager::IndexToVector(dstIndex));
							LegalUnitArray.Add(unit);
						}
						
						if ((*actions)[i].ActionType == EActionEnum::AE_TAKE)
						{
							AChessUnitActor* unit =
								GameManager->GetChessUnitByIndex(!state->isFirstPlayer(), dstIndex);
							if (unit)
							{
								FVector unitLocation = unit->GetActorLocation();
								unitLocation.Z += 30.f;

								unit->LerpMoveToTarget(unitLocation);

								TakeUnitArray.Add(unit);
							}
						}
						else if ((*actions)[i].ActionType == EActionEnum::AE_CASTLING)
						{
							int lookIndex;

							if ((*actions)[i].Action == ACTION_KING_SIDE_CASTLING)
								if (state->isFirstPlayer())
									lookIndex = 63;
								else
									lookIndex = 7;
							else
								if (state->isFirstPlayer())
									lookIndex = 56;
								else
									lookIndex = 0;

							FVector lookPos = AChessGameManager::IndexToVector(lookIndex);
							AChessUnitActor* CastlingRookLegalUnit = GetWorld()->SpawnActor<AChessUnitActor>(lookPos, FRotator::ZeroRotator);
							CastlingRookLegalUnit->InitializeActor(EUnitEnum::UE_ROOK, hitUnit->isWhite, true);
							lookPos.Z += 30.f;
							CastlingRookLegalUnit->LerpMoveToTarget(lookPos);
							LegalUnitArray.Add(CastlingRookLegalUnit);
							CastlingRookLegalUnitArray.Add(CastlingRookLegalUnit);
						}
						else if ((*actions)[i].ActionType == EActionEnum::AE_PROMOTION)
						{
							int promotionIndex = 0;
							if (state->isFirstPlayer())
								promotionIndex = 56 + (*actions)[i].Action - ACTION_PROMOTION_WHITE_START;
							else
								promotionIndex = (*actions)[i].Action - ACTION_PROMOTION_BLACK_START;

							AChessUnitActor* unit = GetWorld()->SpawnActor<AChessUnitActor>(
								actorLocation, FRotator::ZeroRotator);
							unit->InitializeActor(EUnitEnum::UE_QUEEN, hitUnit->isWhite, true);
							unit->LerpMoveToTarget(AChessGameManager::IndexToVector(dstIndex));
							LegalUnitArray.Add(unit);
						}
						else if ((*actions)[i].ActionType == EActionEnum::AE_EN_PASSANT)
						{
							AChessUnitActor* unit =
								GameManager->GetChessUnitByIndex(
									!state->isFirstPlayer(), dstIndex + (state->isFirstPlayer() ? 8 : -8));
							if (unit)
							{
								FVector unitLocation = unit->GetActorLocation();
								unitLocation.Z += 30.f;

								unit->LerpMoveToTarget(unitLocation);

								TakeUnitArray.Add(unit);
							}
						}
					}

					delete actions;
				}
			}
			else
			{
				if (LegalUnitArray.Num() != 0 && !IsRunTimer)
				{
					IsRunTimer = true;
					LegalTargetLocation = actorLocation;
					SelectedLegalUnit = hitUnit;

					for (int i = 0; i < LegalUnitArray.Num(); i++)
					{
						LegalUnitArray[i]->LerpMoveToTargetReverse();
					}

					for (int i = 0; i < TakeUnitArray.Num(); i++)
					{
						FVector TakeUnitLocation = LegalTargetLocation;
						TakeUnitLocation.Z += 30.f;

						int legalTargetIndex = AChessGameManager::VectorToIndex(LegalTargetLocation);
						if (!TakeUnitArray[i]->GetActorLocation().Equals(TakeUnitLocation))
							TakeUnitArray[i]->LerpMoveToTargetReverse();
						else
						{
							if (!GameManager->GetChessUnitByIndex(!state->isFirstPlayer(), legalTargetIndex))
								GameManager->MoveTakeUnitToDeadSpace(legalTargetIndex + (state->isFirstPlayer() ? 8 : -8));
							else
								GameManager->MoveTakeUnitToDeadSpace(legalTargetIndex);
						}
					}

					TakeUnitArray.Empty();

					FTimerHandle WaitHandle;
					GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
						{
							AChessUnitActor* CastlingRookLegalUnit = 0;
							for (int i = 0; i < CastlingRookLegalUnitArray.Num(); i++)
							{
								if (CastlingRookLegalUnitArray[i] == SelectedLegalUnit)
									CastlingRookLegalUnit = CastlingRookLegalUnitArray[i];
							}

							State* state = GetWorld()->GetGameState<AChessGameStateBase>()->mState;
							if (CastlingRookLegalUnit)
							{
								int rookIndex = AChessGameManager::VectorToIndex(CastlingRookLegalUnit->GetActorLocation());
								AChessUnitActor* lookUnit = GameManager->GetChessUnitByIndex(state->isFirstPlayer(), rookIndex);
								if (rookIndex == 0)
								{
									SelectedUnit->LerpMoveToTarget(AChessGameManager::IndexToVector(2));
									lookUnit->LerpMoveToTarget(AChessGameManager::IndexToVector(3));
									GameManager->NextState(ACTION_QUEEN_SIDE_CASTLING);
								}
								else if (rookIndex == 7)
								{
									SelectedUnit->LerpMoveToTarget(AChessGameManager::IndexToVector(6));
									lookUnit->LerpMoveToTarget(AChessGameManager::IndexToVector(5));
									GameManager->NextState(ACTION_KING_SIDE_CASTLING);
								}
								else if (rookIndex == 56)
								{
									SelectedUnit->LerpMoveToTarget(AChessGameManager::IndexToVector(58));
									lookUnit->LerpMoveToTarget(AChessGameManager::IndexToVector(59));
									GameManager->NextState(ACTION_QUEEN_SIDE_CASTLING);
								}
								else
								{
									SelectedUnit->LerpMoveToTarget(AChessGameManager::IndexToVector(62));
									lookUnit->LerpMoveToTarget(AChessGameManager::IndexToVector(61));
									GameManager->NextState(ACTION_KING_SIDE_CASTLING);
								}
							}
							else
							{
								if (SelectedUnit->UnitType == EUnitEnum::UE_PAWN &&
									SelectedLegalUnit->UnitType == EUnitEnum::UE_QUEEN)
								{
									SelectedUnit->InitializeActor(EUnitEnum::UE_QUEEN, SelectedUnit->isWhite, false);
								}
								SelectedUnit->LerpMoveToTarget(LegalTargetLocation);

								GameManager->NextState(State::indexToAction(
									AChessGameManager::VectorToIndex(SelectedUnit->GetActorLocation()),
									AChessGameManager::VectorToIndex(LegalTargetLocation)));
							
								if (state->isCheckMate())
									GameManager->CheckMate();
								else
									GameManager->ChangeViewTarget(1.f);
							}

							for (int i = 0; i < LegalUnitArray.Num(); i++)
							{
								LegalUnitArray[i]->Destroy();
							}

							LegalUnitArray.Empty();
							CastlingRookLegalUnitArray.Empty();

							IsRunTimer = false;

						}), 0.98f, false);
				}
			}

			return;
		}
	}

	if (LegalUnitArray.Num() != 0 && !IsRunTimer)
	{
		IsRunTimer = true;
		for (int i = 0; i < LegalUnitArray.Num(); i++)
		{
			LegalUnitArray[i]->LerpMoveToTargetReverse();
		}

		for (int i = 0; i < TakeUnitArray.Num(); i++)
		{
			TakeUnitArray[i]->LerpMoveToTargetReverse();
		}

		TakeUnitArray.Empty();

		FTimerHandle WaitHandle;
		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
			{
				for (int i = 0; i < LegalUnitArray.Num(); i++)
				{
					LegalUnitArray[i]->Destroy();
				}

				LegalUnitArray.Empty();
				IsRunTimer = false;

			}), 0.98f, false);
	}
}

// Called every frame
void AChessPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}