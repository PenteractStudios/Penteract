#include "Barrel.h"
#include "GameplaySystems.h"
#include "Components/Physics/ComponentSphereCollider.h"
#include "CameraController.h"
#include "Components/ComponentParticleSystem.h"

EXPOSE_MEMBERS(Barrel) {
	// Add members here to expose them to the engine. Example:
	MEMBER(MemberType::FLOAT, timeToDestroy),
	MEMBER(MemberType::FLOAT, timeWillDoDamage),
	MEMBER(MemberType::FLOAT, timerToDestroy),
	MEMBER(MemberType::BOOL, onFloor),
	MEMBER(MemberType::FLOAT, forceOfFall),
	MEMBER(MemberType::FLOAT, shakeMultiplier)
};

GENERATE_BODY_IMPL(Barrel);

void Barrel::Start() {
	barrel = &GetOwner();
	parentTransform = barrel->GetParent()->GetComponent<ComponentTransform>();

	barrelMesh = barrel->GetParent()->GetChild("BarrelMesh");

	barrelCollider = barrel->GetParent()->GetChild("Barrel");
	if (barrelCollider) {
		barrelCollider = barrelColliderAux;
		barrelCollider->Disable();
	}

	GameObject* cameraAux = GameplaySystems::GetGameObject("Game Camera"); 
	if (cameraAux) {
		cameraController = GET_SCRIPT(cameraAux, CameraController);
	}

	GameObject* particleAux = barrel->GetParent()->GetChild("ShockWaveGround");
	if (particleAux) {
		particles = particleAux->GetComponent<ComponentParticleSystem>();
		audio = particleAux->GetComponent<ComponentAudioSource>();
	}

	GameObject* particleForTimerAux = barrel->GetParent()->GetChild("BarrelWarning");
	if (particleForTimerAux) {
		particlesForTimer = particleForTimerAux->GetComponent<ComponentParticleSystem>();
		audioForTimer = particleForTimerAux->GetComponent<ComponentAudioSource>();
	}
}

void Barrel::Update() {
	if (!barrel || !barrelCollider || !cameraController || !particles || !audio || !particlesForTimer || !audioForTimer) return;

	if (startTimerToDestroy && timerDestroyActivated) {
		particlesForTimer->PlayChildParticles();
		audioForTimer->Play();

		currentTimerToDestroy += Time::GetDeltaTime();
		if (currentTimerToDestroy >= timerToDestroy) {
			audioForTimer->Stop();
			isHit = true;
			startTimerToDestroy = false;
		}

	}

	if (isHit) {
		barrelCollider->Enable();
		isHit = false;
		particles->PlayChildParticles();
		audio->Play();
		barrelMesh->Disable();
		destroy = true;
		cameraController->StartShake(shakeMultiplier);
	}

	if (destroy) {
		if (timeWillDoDamage > 0) {
			timeWillDoDamage -= Time::GetDeltaTime();
		}
		else {
			if (barrelCollider) barrelCollider->Disable();
		}

		if (timeToDestroy > 0) {
			timeToDestroy -= Time::GetDeltaTime();
		}
		else {
			destroy = false;
			GameplaySystems::DestroyGameObject(barrel->GetParent());
		}
	}

	if (!onFloor) {
		float3 barrelPos = parentTransform->GetGlobalPosition();
		if (barrelPos.y > 3) {
			barrelPos += float3(0, -forceOfFall, 0);
			parentTransform->SetGlobalPosition(barrelPos);
		}
		else {
			startTimerToDestroy = true;
			timerDestroyActivated = true;
			onFloor = true;
		}
	}
}

void Barrel::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle)
{
	if (particle) {
		ComponentParticleSystem::Particle* p = (ComponentParticleSystem::Particle*)particle;
		ComponentParticleSystem* pSystem = collidedWith.GetComponent<ComponentParticleSystem>();
		if (pSystem) pSystem->KillParticle(p);

		if ( collidedWith.name == "FangRightBullet" || collidedWith.name == "FangLeftBullet" || collidedWith.name == "OnimaruBullet"  ) {

			if (!timerDestroyActivated) {
				startTimerToDestroy = true;
				timerDestroyActivated = true;
			}

			ComponentSphereCollider* thisCollider = GetOwner().GetComponent<ComponentSphereCollider>();
			thisCollider->Disable();
		}

	}

}