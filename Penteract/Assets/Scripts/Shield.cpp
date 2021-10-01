#include "Shield.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "PlayerController.h"
#include "RangerProjectileScript.h"
#include "Components/Physics/ComponentSphereCollider.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAgent.h"
#include "Math/float3.h"

EXPOSE_MEMBERS(Shield) {
	MEMBER(MemberType::INT, maxCharges),
	MEMBER(MemberType::FLOAT, chargeCooldown),
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, particlesColliderUID),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, particlesUpgradeColliderUID),
	MEMBER_SEPARATOR("Debug only"),
	MEMBER(MemberType::INT, currentAvailableCharges)
};

GENERATE_BODY_IMPL(Shield);

void Shield::Start() {
	currentAvailableCharges = maxCharges;
	GameObject* playerGO = GameplaySystems::GetGameObject(playerUID);
	if (playerGO) playerController = GET_SCRIPT(playerGO, PlayerController);
	audio = GetOwner().GetComponent<ComponentAudioSource>();
	particlesCollider = GameplaySystems::GetResource<ResourcePrefab>(particlesColliderUID);
	particlesReboundCollider = GameplaySystems::GetResource<ResourcePrefab>(particlesUpgradeColliderUID);
}

void Shield::Update() {}

void Shield::InitShield() {
	isActive = true;
}

void Shield::FadeShield() {
	isActive = false;
}

void Shield::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle) {
	if ((collidedWith.name == "WeaponParticles" || collidedWith.name == "RightBlade" || collidedWith.name == "LeftBlade") && isActive && playerController) {

		if (!particle) {
			collidedWith.Disable();
		} else {
			ComponentParticleSystem::Particle* p = (ComponentParticleSystem::Particle*)particle;
			ComponentParticleSystem* pSystem = collidedWith.GetComponent<ComponentParticleSystem>();
			float3 position = p->position;
			Quat rotation = GetOwner().GetComponent<ComponentTransform>()->GetGlobalRotation()  * float3x3::FromEulerXYZ(pi / 2, 0.0f, 0.0f).ToQuat();

			if (playerController->playerOnimaru.level1Upgrade && collidedWith.name == "WeaponParticles") {		// Reflect projectile
				if (!particle) return;
				// Separate Bullet from shield
				float3 actualPenDistance = penetrationDistance.ProjectTo(collisionNormal);
				p->position = p->position + actualPenDistance;
				// Reflect
				float3 front = p->direction;
				float3 normal = -float3(collisionNormal.x, 0, collisionNormal.z);
				float3 newFront = front - 2 * front.ProjectTo(normal);
				p->direction = newFront;
				// Convert to player Bullet
				pSystem->layer = WorldLayers::BULLET;
				pSystem->layerIndex = 5;
				Physics::UpdateParticleRigidbody(p);
				pSystem->layer = WorldLayers::BULLET_ENEMY;
				pSystem->layerIndex = 6;

				if (particlesReboundCollider)GameplaySystems::Instantiate(particlesReboundCollider, position, rotation);
			} else {
				if (particlesCollider)GameplaySystems::Instantiate(particlesCollider, position, rotation);
			}
			if (pSystem) pSystem->KillParticle(p);
		}

		currentAvailableCharges--;

		if (audio) {		// Play hit effect
			audio->Play();
		}
	} else if (collidedWith.name == "MeleeGrunt" || collidedWith.name == "RangedGrunt") {
		ComponentAgent* agent = collidedWith.GetComponent<ComponentAgent>();
		if (agent) {
			agent->RemoveAgentFromCrowd();
			float3 actualPenDistance = -penetrationDistance.ProjectTo(collisionNormal);
			collidedWith.GetComponent<ComponentTransform>()->SetGlobalPosition(collidedWith.GetComponent<ComponentTransform>()->GetGlobalPosition() + actualPenDistance);
			agent->AddAgentToCrowd();
		}
	}
}