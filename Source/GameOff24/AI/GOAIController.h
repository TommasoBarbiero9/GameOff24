// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GOAIController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEOFF24_API AGOAIController : public AAIController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;
		
protected:
	virtual void BeginPlay() override;
};
