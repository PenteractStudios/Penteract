#include "NoodleShop.h"

#include "FactoryDoors.h"
#include "GameplaySystems.h"
#include "GameObject.h"

EXPOSE_MEMBERS(NoodleShop) {
    MEMBER(MemberType::GAME_OBJECT_UID, doorUID),
    MEMBER(MemberType::GAME_OBJECT_UID, noodleShopObstacleUID),
    MEMBER(MemberType::GAME_OBJECT_UID, audioWarningUID)
};

GENERATE_BODY_IMPL(NoodleShop);

void NoodleShop::Start() {
    gameObject = &GetOwner();
	GameObject* door = GameplaySystems::GetGameObject(doorUID);
    if (door) {
        script = GET_SCRIPT(door, FactoryDoors);
    }
}

void NoodleShop::Update() {
	
}

void NoodleShop::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle) {
    if (script) {
        bool correctCollider = collidedWith.name == "FangRightBullet" || collidedWith.name == "FangLeftBullet" || collidedWith.name == "OnimaruBullet" ||
                               collidedWith.name == "Onimaru" || collidedWith.name == "Fang";
        if (correctCollider) {
            if (particle) {
                ComponentParticleSystem::Particle* p = (ComponentParticleSystem::Particle*)particle;
                ComponentParticleSystem* pSystem = collidedWith.GetComponent<ComponentParticleSystem>();
                if (pSystem) pSystem->KillParticle(p);
            }
            ComponentCapsuleCollider* capsuleCollider = gameObject->GetComponent<ComponentCapsuleCollider>();
            if (capsuleCollider) capsuleCollider->Disable();

            GameObject* noodleShopObstacleGO = GameplaySystems::GetGameObject(noodleShopObstacleUID);
            if (noodleShopObstacleGO) {
                script->Open();
                noodleShopObstacleGO->Disable();
            }

            GameObject* audioWarning = GameplaySystems::GetGameObject(audioWarningUID);
            if (audioWarning) {
                ComponentAudioSource* audioWarningComp = audioWarning->GetComponent<ComponentAudioSource>();
                if (audioWarningComp) audioWarningComp->Stop();
            }
        }


    }
}