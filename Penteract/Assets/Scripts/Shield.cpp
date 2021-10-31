#include "Shield.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "PlayerController.h"
#include "RangerProjectileScript.h"
#include "Components/Physics/ComponentSphereCollider.h"
#include "Components/ComponentBillboard.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAgent.h"
#include "AttackDroneProjectile.h"
#include "Math/float3.h"

EXPOSE_MEMBERS(Shield) {
	MEMBER(MemberType::INT, maxCharges),
	MEMBER(MemberType::FLOAT, chargeCooldown),
	MEMBER_SEPARATOR("Life Effect"),
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, ShieldBilboardUID),
	MEMBER(MemberType::FLOAT, maxFrames),
	MEMBER_SEPARATOR("Shield Impact"),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, particlesColliderUID),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, particlesUpgradeColliderUID),
	MEMBER(MemberType::FLOAT, rangeBulletLifeRebound),
	MEMBER(MemberType::GAME_OBJECT_UID, ShieldBilboardUID),
	MEMBER_SEPARATOR("Debug only"),
	MEMBER(MemberType::INT, currentAvailableCharges),
	MEMBER(MemberType::FLOAT, shieldMaxScale),
	MEMBER(MemberType::FLOAT, growthSpeed),
	MEMBER(MemberType::FLOAT, fadeSpeed),
	MEMBER(MemberType::FLOAT, growthThreshold),
	MEMBER(MemberType::FLOAT, fadeThreshold)
};

GENERATE_BODY_IMPL(Shield);

void Shield::Start() {
	currentAvailableCharges = maxCharges;
	GameObject* playerGO = GameplaySystems::GetGameObject(playerUID);
	if (playerGO) playerController = GET_SCRIPT(playerGO, PlayerController);
	audio = GetOwner().GetComponent<ComponentAudioSource>();
	particlesCollider = GameplaySystems::GetResource<ResourcePrefab>(particlesColliderUID);

	transform = GetOwner().GetComponent<ComponentTransform>();

	GameObject* bilboAux = GameplaySystems::GetGameObject(ShieldBilboardUID);
	if (bilboAux)shieldBilb = bilboAux->GetComponent<ComponentBillboard>();
	particlesReboundCollider = GameplaySystems::GetResource<ResourcePrefab>(particlesUpgradeColliderUID);
}

void Shield::Update() {
	switch (shieldState) {
	case  ShieldState::OFFLINE:
		break;
	case  ShieldState::GROWING:
		if (transform->GetScale().x < shieldMaxScale - growthThreshold) {
			transform->SetScale(float3(float3::Lerp(transform->GetScale(), float3(shieldMaxScale), Time::GetDeltaTime() * growthSpeed)));
		}
		else {
			transform->SetScale(float3(shieldMaxScale));
			shieldState = ShieldState::IDLE;
		}
		break;
	case  ShieldState::IDLE:
		break;
	case  ShieldState::FADING:
		if (transform->GetScale().x > fadeThreshold) {
			transform->SetScale(float3(float3::Lerp(transform->GetScale(), float3(0.0f), Time::GetDeltaTime() * fadeSpeed)));
		}
		else {
			transform->SetScale(float3(0.1f));
			shieldState = ShieldState::OFFLINE;
			GetOwner().Disable();
		}
		break;
	}

	if (currentAvailableCharges < 1) {
		if (GetOwner().GetComponent<ComponentMeshRenderer>()->HasDissolveAnimationFinished()) {
			GetOwner().Disable();
		}
	}else{
		currentFrame = maxCharges - currentAvailableCharges;
		shieldBilb->SetCurrentFrame(currentFrame);
	}
}

void Shield::InitShield() {
	isActive = true;
	currentFrame = 0;
	shieldState = ShieldState::GROWING;
	transform->SetScale(float3(0.01f));
	GetOwner().GetComponent<ComponentMeshRenderer>()->ResetDissolveValues();
	GetOwner().Enable();
}

void Shield::FadeShield() {
	if (currentAvailableCharges < 1) {
		GetOwner().GetComponent<ComponentMeshRenderer>()->PlayDissolveAnimation();
	}
	else {
		shieldState = ShieldState::FADING;
	}
	isActive = false;
}


void Shield::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle) {
	if ((collidedWith.name == "BulletRange" || collidedWith.name == "DukeProjectile" || collidedWith.name == "RightBlade" || collidedWith.name == "LeftBlade" || collidedWith.name == "AttackDroneProjectile") && isActive && playerController) {

		if (!particle) {
			collidedWith.Disable();
		} else {
			ComponentParticleSystem::Particle* p = (ComponentParticleSystem::Particle*)particle;
			ComponentParticleSystem* pSystem = collidedWith.GetComponent<ComponentParticleSystem>();
			float3 position = pSystem->GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition();
			Quat rotation = GetOwner().GetComponent<ComponentTransform>()->GetGlobalRotation()  * float3x3::FromEulerXYZ(pi / 2, 0.0f, 0.0f).ToQuat();
			float3 front = collidedWith.GetComponent<ComponentTransform>()->GetGlobalRotation() * float3(0.0f, 0.f, 1.f);
			//if (pSystem) pSystem->KillParticle(p);

			if (collidedWith.name == "BulletRange" && playerController->playerFang.level1Upgrade) {		// Reflect projectile
				if (!particle) return;
				// Reflect
				//TODO MAY LOWY NEED IT TO IMPROVE TO REBOUND
				/*float3 normal = -float3(collisionNormal.x, 0, collisionNormal.z);
				float3 newFront = (front - 2 * front.ProjectTo(normal)).Normalized();
				p->direction = newFront;*/
				RangerProjectileScript* rangeBulletScript = GET_SCRIPT(&collidedWith, RangerProjectileScript);
				/*float3 right = Cross(newFront, float3(0.0f, 1.f, 0.f)).Normalized();
				float3x3 newMatrix;
				newMatrix.SetCol(0, right);
				newMatrix.SetCol(1, float3(0.0f, 1.f, 0.f));
				newMatrix.SetCol(2, newFront);*/
				if (rangeBulletScript) {
					//collidedWith.GetComponent<ComponentTransform>()->SetGlobalRotation(collidedWith.GetComponent<ComponentTransform>()->GetGlobalRotation().Inverted());
					rangeBulletScript->SetSpeed(-rangeBulletScript->GetSpeed());
					rangeBulletScript->life = rangeBulletLifeRebound;
				}
				// Convert to player Bullet
				pSystem->layer = WorldLayers::BULLET;
				pSystem->layerIndex = 5;
				Physics::UpdateParticleRigidbody(p);
	

				if (particlesReboundCollider)GameplaySystems::Instantiate(particlesReboundCollider, position, rotation);
			} else {
				if (particlesCollider)GameplaySystems::Instantiate(particlesCollider, position, rotation);
				if (pSystem) pSystem->KillParticle(p);
			}

			AttackDroneProjectile* projectileScript = GET_SCRIPT(&collidedWith, AttackDroneProjectile);
			if (projectileScript) projectileScript->Collide();
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
	} else if (collidedWith.name == "DukeCharge") {
		currentAvailableCharges = 0;
	}
}