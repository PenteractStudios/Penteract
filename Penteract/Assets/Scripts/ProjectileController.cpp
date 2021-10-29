#include "ProjectileController.h"

#include "GameplaySystems.h"

GameObject* ProjectileController::Shoot(UID bulletPrefabUID, float3 position, Quat rotation) {
	if (bulletPrefabUID == 0) return nullptr;

	ResourcePrefab* bulletPrefab = GameplaySystems::GetResource<ResourcePrefab>(bulletPrefabUID);
	if (!bulletPrefab) return nullptr;

	GameObject* projectileInstance(GameplaySystems::Instantiate(bulletPrefab, position, rotation));
	return projectileInstance;
}