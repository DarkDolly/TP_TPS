// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/AIControllerBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "TP_TPS/Public/Classes/CharacterBase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer)
{
	BBC = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	BTC = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree Component"));
}

void AAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ACharacterBase* Chr = Cast<ACharacterBase>(InPawn);
	if (Chr != nullptr && Chr->TreeAsset != nullptr)
	{
		// Initialize the Blackboard
		BBC->InitializeBlackboard(*Chr->TreeAsset->BlackboardAsset);

		// Set Blackboard Key IDs
		EnemyKeyId = BBC->GetKeyID("TargetActor");

		// Start the BehaviorTree
		BTC->StartTree(*Chr->TreeAsset);
	}
}