#include "Barrel.h"
#include "GameplaySystems.h"
#include "Components/Physics/ComponentSphereCollider.h"
#include "CameraController.h"
#include "Components/ComponentParticleSystem.h"

EXPOSE_MEMBERS(Barrel) {
	// Add members here to expose them to the engine. Example:
	MEMBER(MemberType::GAME_OBJECT_UID, barrelUID),
	MEMBER(MemberType::GAME_OBJECT_UID, sphereColliderUID),
	MEMBER(MemberType::GAME_OBJECT_UID, cameraUID),
	MEMBER(MemberType::GAME_OBJECT_UID, particlesUID),
	MEMBER(MemberType::GAME_OBJECT_UID, particlesForTimerUID),
	MEMBER(MemberType::FLOAT, timeToDestroy),
	MEMBER(MemberType::FLOAT, timeToDestroyCollider),
	MEMBER(MemberType::FLOAT, timerToDestroy),
	MEMBER(MemberType::FLOAT, shakeMultiplier)
};

GENERATE_BODY_IMPL(Barrel);

void Barrel::Start() {
	barrel = GameplaySystems::GetGameObject(barrelUID);

	barrelCollider = GameplaySystems::GetGameObject(sphereColliderUID);
	if (barrelCollider) {
		barrelCollider->Disable();
	}

	GameObject* cameraAux = GameplaySystems::GetGameObject(cameraUID);
	if (cameraAux) {
		cameraController = GET_SCRIPT(cameraAux, CameraController);
	}
	GameObject* particleAux = GameplaySystems::GetGameObject(particlesUID);
	if (particleAux) {
		particles = particleAux->GetComponent<ComponentParticleSystem>();
		audio = particleAux->GetComponent<ComponentAudioSource>();
	}

	GameObject* particleForTimerAux = GameplaySystems::GetGameObject(particlesForTimerUID);
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
		barrel->Disable();
		destroy = true;
		cameraController->StartShake(shakeMultiplier);
	}

	if (destroy) {

		if (timeToDestroy > 0) {
			if (timeToDestroy <= timeToDestroyCollider && barrelCollider->IsActive()) barrelCollider->Disable();
			timeToDestroy -= Time::GetDeltaTime();
		}
		else {
			destroy = false;
			GameplaySystems::DestroyGameObject(barrel->GetParent());
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