#include "Onimaru.h"

Onimaru::Onimaru(int lifePoints_, float movementSpeed_, int damageHit_, float attackSpeed_, UID onimaruUID, UID onimaruBulletUID, UID onimaruGunUID, UID onimaruTrailUID)
{
	attackSpeed = attackSpeed_;
	lifePoints = lifePoints_;
	movementSpeed = movementSpeed_;
	damageHit = damageHit_;
	SetTotalLifePoints(lifePoints);

	onimaru = GameplaySystems::GetGameObject(onimaruUID);

	if (onimaru && onimaru->GetParent()) {
		characterTransform = onimaru->GetParent()->GetComponent<ComponentTransform>();
		agent = onimaru->GetParent()->GetComponent<ComponentAgent>();
		compAnimation = onimaru->GetComponent<ComponentAnimation>();

		if (agent) {
			agent->SetMaxSpeed(movementSpeed);
			agent->SetMaxAcceleration(MAX_ACCELERATION);
		}
	}

	GameObject* onimaruGun = GameplaySystems::GetGameObject(onimaruGunUID);
	if (onimaruGun) {
		onimaruGunTransform = onimaruGun->GetComponent<ComponentTransform>();
	}

	onimaruBullet = GameplaySystems::GetResource<ResourcePrefab>(onimaruBulletUID);

	onimaruTrail = GameplaySystems::GetResource<ResourcePrefab>(onimaruTrailUID);
}
