// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

USTRUCT(BlueprintType)
struct FAnimValues
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = Movement)
		bool bIsCrouching = false;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
		bool bIsInCombat = false;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
		bool bIsShooting = false;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
		bool bADS = false;

};

UCLASS()
class TP_TPS_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

	// Behavior Tree Asset
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* TreeAsset;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FAnimValues AnimValues;

	UFUNCTION(BlueprintCallable)
	void ToggleCombat(const bool Newbool);

	UFUNCTION(BlueprintCallable)
	void ToggleCrouch(const bool Newbool);

	UFUNCTION(BlueprintCallable)
	void ToggleADS(const bool Newbool);

	UFUNCTION(BlueprintCallable)
	void ToggleSprinting(bool Newbool);

	UFUNCTION(BlueprintCallable)
	void AttachWeapon(USkeletalMeshComponent* WeaponMesh, const FName SocketName);

	UFUNCTION(BlueprintCallable)
	void StartWeaponFire();

	UFUNCTION(BlueprintCallable)
	void StopWeaponFire();

	UFUNCTION(BlueprintCallable)
	void PlayMontage(UAnimMontage* Montage, float Rate);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* Eventinstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void MoveForward(float Val);
	UFUNCTION(BlueprintCallable)
	void MoveRight(float Val);
	UFUNCTION(BlueprintCallable)
	void LookUp(float Val);
	UFUNCTION(BlueprintCallable)
	void Turn(float Val);

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
	class USkeletalMeshComponent* Weapon;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
	UParticleSystem* BloodFX;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
	UParticleSystem* BulletImpact;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float SlowWalkSpeed = 94.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 375.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 94.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float CrouchedWalkSpeed = 100.f;

	FTimerHandle FireHandle;

	FHitResult TraceProvider(FVector Start, FVector End);

	float Health = 100.f;
};
