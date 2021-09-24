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
		MEMBER(MemberType::FLOAT, forceOfFall)

};

GENERATE_BODY_IMPL(Barrel);

void Barrel::Start() {
	barrel = &GetOwner();
	parentTransform = barrel->GetParent()->GetComponent<ComponentTransform>();

	barrelMesh = barrel->GetParent()->GetChild("BarrelMesh");

	GameObject* barrelColliderAux = barrel->GetParent()->GetChild("Barrel");
	if (barrelColliderAux) {
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

	if (startTimerToDestroy && timerDestroyActivated) {
		if (particlesForTimer) particlesForTimer->PlayChildParticles();
		if (audioForTimer) audioForTimer->Play();

		currentTimerToDestroy += Time::GetDeltaTime();
		if (currentTimerToDestroy >= timerToDestroy) {
			if (audioForTimer) audioForTimer->Stop();
			isHit = true;
			startTimerToDestroy = false;
		}

	}

	if (isHit) {
		if(barrelCollider) barrelCollider->Enable();
		isHit = false;
		if(particles) particles->PlayChildParticles();
		if(audio) audio->Play();
		if(barrelMesh) barrelMesh->Disable();
		destroy = true;
		if(cameraController) cameraController->StartShake();
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
			if(barrel) GameplaySystems::DestroyGameObject(barrel->GetParent());
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