#include "DukeShield.h"

#include "GameObject.h"
#include "GameplaySystems.h"
#include "AIDuke.h"
#include "RangerProjectileScript.h"
#include "Components/Physics/ComponentSphereCollider.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentTransform.h"
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
}

void DukeShield::FadeShield() {
	shieldState = ShieldState::FADING;
	isActive = false;
}

void DukeShield::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle) {
	if ((collidedWith.name == "OnimaruBullet" || collidedWith.name == "FangLeftBullet" || collidedWith.name == "FangRightBullet" ||
		collidedWith.name == "OnimaruBulletUltimate" || collidedWith.name == "FangBullet")
		&& isActive && duke) {

		if (!particle) {
			Debug::Log("Disable");
			collidedWith.Disable();
		} else {
			ComponentParticleSystem::Particle* p = (ComponentParticleSystem::Particle*)particle;
			ComponentParticleSystem* pSystem = collidedWith.GetComponent<ComponentParticleSystem>();
			if (pSystem) pSystem->KillParticle(p);
			Debug::Log("Kill");
		}

		if (audio) {		// Play hit effect
			audio->Play();
		}
	}
}