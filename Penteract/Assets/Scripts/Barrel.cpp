#include "Barrel.h"
#include "GameplaySystems.h"
#include "Components/Physics/ComponentSphereCollider.h"
#include "CameraController.h"
#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentObstacle.h"

EXPOSE_MEMBERS(Barrel) {
	// Add members here to expose them to the engine. Example:
	MEMBER(MemberType::FLOAT, timeToDestroy),
	MEMBER(MemberType::FLOAT, timeWillDoDamage),
	MEMBER(MemberType::FLOAT, timerToDestroy),
	MEMBER(MemberType::BOOL, onFloor),
	MEMBER(MemberType::FLOAT, heightOfThrow),
	MEMBER(MemberType::FLOAT, forceOfFall),
	MEMBER(MemberType::FLOAT, shakeMultiplier)
};

GENERATE_BODY_IMPL(Barrel);

void Barrel::Start() {
	barrel = &GetOwner();
	parentTransform = barrel->GetParent()->GetComponent<ComponentTransform>();

	barrelMesh = barrel->GetParent()->GetChild("BarrelMesh");
	if (barrelMesh) {
		obstacle = barrelMesh->GetComponent<ComponentObstacle>();
		if(!onFloor && obstacle) obstacle->Disable();
	}

	barrelCollider = barrel->GetParent()->GetChild("Barrel");
	if (barrelCollider) {
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

	GameObject* particleFlameLightAux = barrel->GetParent()->GetChild("MissileFlameLight");
	if (particleFlameLightAux) {
		particlesFlameLight = particleFlameLightAux->GetComponent<ComponentParticleSystem>();
		audioFlameLight = particleFlameLightAux->GetComponent<ComponentAudioSource>();
	}

	if (!onFloor) {
		float3 pos = parentTransform->GetGlobalPosition();
		parentTransform->SetGlobalPosition(float3(pos.x, heightOfThrow, pos.z));
	}

	barrelShadow = barrel->GetParent()->GetParent()->GetChild("BarrelShadowWarning");
	if (barrelShadow) {
		particlesShadow = barrelShadow->GetComponent<ComponentParticleSystem>();
		if (particlesShadow) {
			if (!onFloor) particlesShadow->PlayChildParticles();
		}
	}
}

void Barrel::Update() {
	if (!barrelMesh || !barrelCollider || !cameraController || !particles || !audio ) return;

	if (startTimerToDestroy && timerDestroyActivated) {
		particlesForTimer->PlayChildParticles();
		//audioForTimer->Play(); // TODO uncomment this line when BarrelWarning have is proper sound (right now doesn't have and produce bugs)

		currentTimerToDestroy += Time::GetDeltaTime();
		if (currentTimerToDestroy >= timerToDestroy) {
			//audioForTimer->Stop(); // TODO uncomment this line when BarrelWarning have is proper sound (right now doesn't have and produce bugs)
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
			if (particlesShadow) {
				GameplaySystems::DestroyGameObject(barrel->GetParent()->GetParent());
			}
			else {
				GameplaySystems::DestroyGameObject(barrel->GetParent());
			}
		}
	}

	if (!onFloor) {
		if (parentTransform->GetGlobalPosition().y > 0.8f) {
			float3 barrelPos = parentTransform->GetGlobalPosition();
			barrelPos += float3(0, -forceOfFall, 0);
			parentTransform->SetGlobalPosition(barrelPos);
		}
		else {
			float3 barrelPos = parentTransform->GetGlobalPosition();
			barrelPos.y = 0.8f;
			parentTransform->SetGlobalPosition(barrelPos);
			//startTimerToDestroy = true;
			//timerDestroyActivated = true;
			isHit = true;
			onFloor = true;
			//obstacle->Enable();
			if (particlesShadow) {
				particlesShadow->StopChildParticles();
			}
			if (particlesFlameLight) {
				particlesFlameLight->SetParticlesPerSecondChild(float2(0.f,0.f));
			}
		}
	}
}

void Barrel::OnCollision(GameObject& collidedWith, float3 /* collisionNormal */, float3 /* penetrationDistance */, void* particle)
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