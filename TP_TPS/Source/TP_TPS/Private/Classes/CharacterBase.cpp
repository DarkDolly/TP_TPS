// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, "spine_03Socket");
}

void ACharacterBase::MoveForward(float Val)
{
	if ((Controller == NULL) || (Val == 0.0f))
	{
		return;
	}

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Val);
}

void ACharacterBase::MoveRight(float Val)
{
	if ((Controller == NULL) || (Val == 0.0f))
	{
		return;
	}

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Val);
}

void ACharacterBase::LookUp(float Val)
{
	if (Val == 0.f)
	{
		return;
	}
	AddControllerPitchInput(Val);
}

void ACharacterBase::Turn(float Val)
{
	if (Val == 0.f)
	{
		return;
	}
	AddControllerYawInput(Val);
}

FHitResult ACharacterBase::TraceProvider(FVector Start, FVector End)
{
	FHitResult OutHit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);

	return OutHit;
}

void ACharacterBase::ToggleCombat(const bool Newbool)
{
	AnimValues.bIsInCombat = Newbool;
	bUseControllerRotationYaw = Newbool;
	GetCharacterMovement()->bOrientRotationToMovement = !Newbool;
	FName NewSocket = Newbool ? "hand_rSocket" : "spine_03Socket";
	AttachWeapon(Weapon, NewSocket);
	GetCharacterMovement()->MaxWalkSpeed = (Newbool) ? 187.f : 94.f;
}

void ACharacterBase::ToggleCrouch(const bool Newbool)
{
	AnimValues.bIsCrouching = Newbool;
	const float Speed = AnimValues.bIsInCombat ? 187.f : WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = (Newbool) ? CrouchedWalkSpeed : Speed;
}

void ACharacterBase::ToggleADS(const bool Newbool)
{
	AnimValues.bADS = Newbool;
}

void ACharacterBase::ToggleSprinting(bool Newbool)
{
	if (Newbool)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		return;
	}
	
	GetCharacterMovement()->MaxWalkSpeed = (AnimValues.bIsInCombat) ? 187.f : WalkSpeed;
}

void ACharacterBase::AttachWeapon(USkeletalMeshComponent* WeaponMesh, const FName SocketName)
{
	if (WeaponMesh)
		WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
}

void ACharacterBase::StartWeaponFire()
{
	if (!AnimValues.bIsInCombat)
		return;

	ToggleADS(true);
	ToggleSprinting(false);
	AnimValues.bIsShooting = true;
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Weapon, "MuzzleFlash");

	FVector EyesLoc;
	FRotator EyesRot;
	GetController()->GetPlayerViewPoint(EyesLoc, EyesRot);

	const FVector End = (EyesRot.Vector() * 2000.f) + EyesLoc;
	FHitResult HitInfo = TraceProvider(EyesLoc, End);

	if (!HitInfo.bBlockingHit)
		return;

	UGameplayStatics::SpawnEmitterAtLocation(this, BulletImpact, HitInfo.Location, FRotator(0.f, 0.f, 0.f), true);
	UGameplayStatics::ApplyPointDamage(HitInfo.GetActor(), 20.f, HitInfo.ImpactPoint, HitInfo, this->GetController(), this, nullptr);

	if (FireHandle.IsValid())
	{return;}
	GetWorldTimerManager().SetTimer(FireHandle, this, &ACharacterBase::StartWeaponFire, 0.1f, true, 0.f);
}

void ACharacterBase::StopWeaponFire()
{
	GetWorldTimerManager().ClearTimer(FireHandle);
	AnimValues.bIsShooting = false;
}

void ACharacterBase::PlayMontage(UAnimMontage* Montage, float Rate)
{
	if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Montage, Rate);
	}
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

float ACharacterBase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* Eventinstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, Eventinstigator, DamageCauser);

	UGameplayStatics::SpawnEmitterAtLocation(this, BloodFX, GetActorLocation());

	if (Health <= 0.f)
		return 0.f;

	FHitResult HitInfo;
	FVector ImpulseDir;
	DamageEvent.GetBestHitInfo(this, DamageCauser, HitInfo, ImpulseDir);

	ACharacterBase* Chr = Cast<ACharacterBase>(HitInfo.GetActor());
	if (!HitInfo.bBlockingHit || Chr == nullptr)
		return 0.f;


	Chr->Health = (HitInfo.BoneName == "head") ? 0.f : Health - Damage;

	if (Chr->Health <= 0.f)
	{
		PlayMontage(DeathMontage, 1.f);
		GetMesh()->bIgnoreRadialForce = true;
		GetMesh()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		return 0.f;
	}

	PlayMontage(HitReactMontage, 3.f);
	return 0.f;
}

