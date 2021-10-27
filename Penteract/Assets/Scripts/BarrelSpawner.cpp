#include "BarrelSpawner.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(BarrelSpawner) {
    MEMBER(MemberType::INT, numberBarrelsRound),
    MEMBER(MemberType::FLOAT, secondsBetweenRounds),

    MEMBER_SEPARATOR("Prefabs UIDs"),
    MEMBER(MemberType::PREFAB_RESOURCE_UID, barrelBossUID)
};

GENERATE_BODY_IMPL(BarrelSpawner);

void BarrelSpawner::Start() {
    GameObject* spawner = &GetOwner();
    std::vector<GameObject*> children = spawner->GetChildren();
    for (GameObject* child : children) {
        positionSpawners.push_back(child->GetComponent<ComponentTransform>()->GetGlobalPosition());
    }

    barrel = GameplaySystems::GetResource<ResourcePrefab>(barrelBossUID);
    timerSapwn = secondsBetweenRounds;
}

void BarrelSpawner::Update() {
    if (spawn) {
        if (timerSapwn < secondsBetweenRounds) {
            timerSapwn += Time::GetDeltaTime();
        }
        else {
            timerSapwn = 0.0f;
            for (int i = 0; i < numberBarrelsRound; i++) {
                GameplaySystems::Instantiate(barrel, positionSpawners[currentBarrel], Quat(0, 0, 0, 1));
                if (currentBarrel + 1 > (int)(positionSpawners.size())) {
                    spawn = false;
                    currentBarrel = 0;
                    timerSapwn = secondsBetweenRounds;
                    break;
                }
                ++currentBarrel;
            }
        }
    }
}

void BarrelSpawner::SpawnBarrels() {
    if (!barrel) return;
    spawn = true;   
}