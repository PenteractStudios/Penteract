#include "Barrel.h"
#include "GameplaySystems.h"
#include "Components/Physics/ComponentSphereCollider.h"
#include "CameraController.h"

EXPOSE_MEMBERS(Barrel) {
	// Add members here to expose them to the engine. Example:
	MEMBER(MemberType::GAME_OBJECT_UID, barrelUID),
	MEMBER(MemberType::GAME_OBJECT_UID, cameraUID)
};

GENERATE_BODY_IMPL(Barrel);

void Barrel::Start() {
	GameObject* barrel = GameplaySystems::GetGameObject(barrelUID);
	if (barrel) {
		barrelCollider = barrel->GetComponent<ComponentSphereCollider>();
	}
	GameObject* cameraAux = GameplaySystems::GetGameObject(cameraUID);
	if (cameraAux) {
		cameraController = GET_SCRIPT(cameraAux, CameraController);
	}
}

void Barrel::Update() {
}

void Barrel::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle)
{
	if (!particle) return;
	Debug::Log("Agh");
	ComponentParticleSystem::Particle* p = (ComponentParticleSystem::Particle*)particle;
	ComponentParticleSystem* pSystem = collidedWith.GetComponent<ComponentParticleSystem>();
	if (pSystem) pSystem->KillParticle(p);
	barrelCollider->Enable();
	if (cameraController) {
		cameraController->StartShake();
	}
}