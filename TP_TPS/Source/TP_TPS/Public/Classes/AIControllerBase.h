// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class TP_TPS_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()
	
public:

	AAIControllerBase(const FObjectInitializer& ObjectInitializer);

	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(transient)
	class UBehaviorTreeComponent* BTC;

	UPROPERTY(transient)
	class UBlackboardComponent* BBC;

	// Blackboard Key IDs
	uint8 EnemyKeyId;

};
