#pragma once

#include "GameObject.h"

class ProjectileController
{
public:
	GameObject* Shoot(UID bulletPrefabUID, float3 position, Quat rotation);
};

