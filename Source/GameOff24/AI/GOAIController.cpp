// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void AGOAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree != nullptr)
	{
		RunBehaviorTree(BehaviorTree);

		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
	}
}
