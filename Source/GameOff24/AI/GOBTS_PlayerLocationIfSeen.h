// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "GOBTS_PlayerLocationIfSeen.generated.h"

/**
 * 
 */
UCLASS()
class GAMEOFF24_API UGOBTS_PlayerLocationIfSeen : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UGOBTS_PlayerLocationIfSeen();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
