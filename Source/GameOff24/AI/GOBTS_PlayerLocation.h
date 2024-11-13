// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "GOBTS_PlayerLocation.generated.h"

/**
 * 
 */
UCLASS()
class GAMEOFF24_API UGOBTS_PlayerLocation : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UGOBTS_PlayerLocation();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
