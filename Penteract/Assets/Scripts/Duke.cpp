#include "Duke.h"

#include "RangerProjectileScript.h"

#include <string>

std::uniform_real_distribution<> rng(-1.0f, 1.0f);

void Duke::Init(UID dukeUID, UID playerUID, UID bulletPrefabUID)
{
	gen = std::minstd_rand(rd());

	SetTotalLifePoints(lifePoints);
	characterGameObject = GameplaySystems::GetGameObject(dukeUID);
	player = GameplaySystems::GetGameObject(playerUID);
	bulletPrefab = GameplaySystems::GetResource<ResourcePrefab>(bulletPrefabUID);

	if (characterGameObject) {
		meshObj = characterGameObject->GetChildren()[0];
		dukeTransform = characterGameObject->GetComponent<ComponentTransform>();
		agent = characterGameObject->GetComponent<ComponentAgent>();
		compAnimation = characterGameObject->GetComponent<ComponentAnimation>();
		if (compAnimation) {
			currentState = compAnimation->GetCurrentState();
		}

		if (agent) {
			agent->AddAgentToCrowd();
			agent->SetMaxSpeed(movementSpeed);
			agent->SetMaxAcceleration(MAX_ACCELERATION);
		}

		if (characterGameObject) {
			characterGameObject->GetComponent<ComponentCapsuleCollider>()->Enable();

			//Get audio sources
			int i = 0;

			for (ComponentAudioSource& src : characterGameObject->GetComponents<ComponentAudioSource>()) {
				if (i < static_cast<int>(DUKE_AUDIOS::TOTAL)) dukeAudios[i] = &src;
				i++;
			}
		}
	}
	movementChangeThreshold = moveChangeEvery;
}

void Duke::ShootAndMove(const float3& playerDirection)
{
	// Shoot
	movementTimer += Time::GetDeltaTime();
	if (movementTimer >= movementChangeThreshold) {
		perpendicular = playerDirection.Cross(float3(0, 1, 0));
		perpendicular = perpendicular * rng(gen);
		perpendicular += playerDirection.Normalized() * (playerDirection.Length() - searchRadius);
		std::string p = perpendicular.ToString();
		Debug::Log(p.c_str());
		movementChangeThreshold = moveChangeEvery + rng(gen);
		movementTimer = 0.f;
	}
	agent->SetMoveTarget(dukeTransform->GetGlobalPosition() + perpendicular);
	Shoot();
	Debug::Log("I'm moving while shooting");
}

void Duke::MeleeAttack()
{
	Debug::Log("Hooryah!");
}

void Duke::ShieldShoot()
{
	Shoot();
	Debug::Log("I'm shielding while shooting at your face");
}

void Duke::BulletHell()
{
	Debug::Log("Bullet hell");
}

void Duke::Charge(DukeState nextState)
{
	if ((dukeTransform->GetGlobalPosition() - chargeTarget).Length() <= 0.1f) {
		state = nextState;
	}
	Debug::Log("Electric Tackle!");
}

void Duke::CallTroops()
{
	Debug::Log("Come, guys!");
}

void Duke::Shoot()
{
	attackTimePool -= Time::GetDeltaTime();
	if (attackTimePool <= 0) {
		if (bulletPrefab) {
			if (!meshObj) return;

			ComponentBoundingBox* box = meshObj->GetComponent<ComponentBoundingBox>();

			float offsetY = (box->GetWorldAABB().minPoint.y + box->GetWorldAABB().maxPoint.y) / 4;

			GameObject* projectileInstance(GameplaySystems::Instantiate(bulletPrefab, characterGameObject->GetComponent<ComponentTransform>()->GetGlobalPosition() + float3(0, offsetY, 0), Quat(0, 0, 0, 0)));

			if (projectileInstance) {
				RangerProjectileScript* rps = GET_SCRIPT(projectileInstance, RangerProjectileScript);
				if (rps && dukeTransform) {
					rps->SetRangerDirection(dukeTransform->GetGlobalRotation());
				}
			}
		}
		attackTimePool = 1.0f / (attackFlurry*attackSpeed);
		if (++attackFlurryCounter == attackFlurry) {
			attackTimePool = 1.0f / attackSpeed;
			attackFlurryCounter = 0;
		}
	}
	Debug::Log("PIUM!");
}
