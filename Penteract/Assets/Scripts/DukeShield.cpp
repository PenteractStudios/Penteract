#include "DukeShield.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "AIDuke.h"
#include "RangerProjectileScript.h"
#include "Components/Physics/ComponentSphereCollider.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentMeshRenderer.h"
#include "GameObjectUtils.h"
#include "Math/float3.h"

EXPOSE_MEMBERS(DukeShield) {
	MEMBER(MemberType::GAME_OBJECT_UID, dukeUID),
	MEMBER_SEPARATOR("Debug only"),
	MEMBER(MemberType::FLOAT, shieldMaxScale),
	MEMBER(MemberType::FLOAT, growthSpeed),
	MEMBER(MemberType::FLOAT, fadeSpeed),
	MEMBER(MemberType::FLOAT, growthThreshold),
	MEMBER(MemberType::FLOAT, fadeThreshold)
};

GENERATE_BODY_IMPL(DukeShield);

void DukeShield::Start() {
	GameObject* dukeGO = GameplaySystems::GetGameObject(dukeUID);
	if (dukeGO) duke = GET_SCRIPT(dukeGO, AIDuke);
	audio = GetOwner().GetComponent<ComponentAudioSource>();
	transform = GetOwner().GetComponent<ComponentTransform>();
	mesh = GetOwner().GetComponent<ComponentMeshRenderer>();
	shieldObstacle = GameObjectUtils::SearchReferenceInHierarchy(&GetOwner(), "ShieldObstacle");
	GetOwner().Disable();
}

void DukeShield::Update() {
	switch (shieldState) {
	case  ShieldState::OFFLINE:
		break;
	case  ShieldState::GROWING:
		if (transform->GetScale().x < shieldMaxScale - growthThreshold) {
			transform->SetScale(float3(float3::Lerp(transform->GetScale(), float3(shieldMaxScale), Time::GetDeltaTime() * growthSpeed)));
		} else {
			transform->SetScale(float3(shieldMaxScale));
			shieldState = ShieldState::IDLE;
			if (shieldObstacle) {
				shieldObstacle->Enable();
			}
		}
		break;
	case  ShieldState::IDLE:
		break;
	case  ShieldState::FADING:
		if (transform->GetScale().x > fadeThreshold) {
			transform->SetScale(float3(float3::Lerp(transform->GetScale(), float3(0.0f), Time::GetDeltaTime() * fadeSpeed)));
		} else {
			transform->SetScale(float3(0.1f));
			shieldState = ShieldState::OFFLINE;
			GetOwner().Disable();
		}
		break;
	}
}

void DukeShield::InitShield() {
	GetOwner().Enable();
	shieldState = ShieldState::GROWING;
	isActive = true;
	if (mesh) {
		mesh->PlayDissolveAnimation(true);
	}
	transform->SetScale(float3(0.01f));
}

void DukeShield::FadeShield() {
	shieldState = ShieldState::FADING;
	isActive = false;
	if (mesh) {
		mesh->PlayDissolveAnimation();
	}

	if (shieldObstacle) {
		shieldObstacle->Disable();
	}
}

void DukeShield::OnCollision(GameObject& collidedWith, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* particle) {
	if ((collidedWith.name == "OnimaruBullet" || collidedWith.name == "FangLeftBullet" || collidedWith.name == "FangRightBullet" ||
		collidedWith.name == "OnimaruBulletUltimate" || collidedWith.name == "FangBullet" || collidedWith.name == "DukeProjectile")
		&& isActive && duke) {

		if (!particle) {
			collidedWith.Disable();
		} else {
			ComponentParticleSystem::Particle* p = (ComponentParticleSystem::Particle*)particle;
			if (p->direction.Dot(transform->GetGlobalRotation() * float3(0,0,1)) > 0.f) return;
			ComponentParticleSystem* pSystem = collidedWith.GetComponent<ComponentParticleSystem>();
			if (pSystem) pSystem->KillParticle(p);
		}

		if (audio) {		// Play hit effect
			audio->Play();
		}
	}
}