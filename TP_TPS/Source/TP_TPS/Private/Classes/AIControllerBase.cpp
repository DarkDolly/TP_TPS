// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/AIControllerBase.h"
#include "Classes/SmartObject.h"
#include "BehaviorTree/BehaviorTree.h"
#include "TP_TPS/Public/Classes/CharacterBase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer)
{
	BBC = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	BTC = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree Component"));
}

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (!Agent)
	{
		ACharacterBase* Chr = Cast<ACharacterBase>(GetPawn());
		if (!Chr)
		{return;}
		Agent = Chr;
	}

	if (Agent->SmartObject)
	{
		FGameplayTag SubTag;
		BTC->SetDynamicSubtree(SubTag, Agent->SmartObject->SubTree);
	}
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
		LocationKeyId = BBC->GetKeyID("MoveToLocation");
		ContactKeyId = BBC->GetKeyID("Contact");

		// Start the BehaviorTree
		BTC->StartTree(*Chr->TreeAsset);
	}
}