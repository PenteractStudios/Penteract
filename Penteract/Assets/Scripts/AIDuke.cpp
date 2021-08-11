#include "AIDuke.h"
#include "AIMovement.h"

EXPOSE_MEMBERS(AIDuke) {
	MEMBER_SEPARATOR("Objects UIDs"),
    MEMBER(MemberType::GAME_OBJECT_UID, dukeUID),
    MEMBER(MemberType::GAME_OBJECT_UID, playerUID),

	MEMBER_SEPARATOR("Duke Atributes"),
	MEMBER(MemberType::FLOAT, dukeCharacter.lifePoints),
	MEMBER(MemberType::FLOAT, dukeCharacter.damageHit),
	MEMBER(MemberType::FLOAT, dukeCharacter.damageBullet),
	MEMBER(MemberType::FLOAT, dukeCharacter.damageCharge),
	MEMBER(MemberType::FLOAT, dukeCharacter.movementSpeed),
	MEMBER(MemberType::FLOAT, dukeCharacter.searchRadius),
	MEMBER(MemberType::FLOAT, dukeCharacter.attackRange),
	MEMBER(MemberType::FLOAT, dukeCharacter.pushBackDistance),
	MEMBER(MemberType::FLOAT, dukeCharacter.pushBackSpeed),

	MEMBER_SEPARATOR("Duke Abilities Variables"),
	MEMBER(MemberType::FLOAT, shieldCooldown),
	MEMBER(MemberType::FLOAT, bulletHellCooldown),
	MEMBER(MemberType::FLOAT, movingTime),

	MEMBER_SEPARATOR("Particles UIDs"),

	MEMBER_SEPARATOR("Prefabs UIDs")
};

GENERATE_BODY_IMPL(AIDuke);

void AIDuke::Start() {
	duke = GameplaySystems::GetGameObject(dukeUID);
	player = GameplaySystems::GetGameObject(playerUID);
	movementScript = GET_SCRIPT(&GetOwner(), AIMovement);

	animation = GetOwner().GetComponent<ComponentAnimation>();
	ownerTransform = GetOwner().GetComponent<ComponentTransform>();
}

void AIDuke::Update() {
	switch (phase) {
	case Phase::PHASE1:
		currentShieldCooldown += Time::GetDeltaTime();
		if (dukeCharacter.lifePoints < 0.85 * dukeCharacter.GetTotalLifePoints() && !activeFireTiles) {
			activeFireTiles = true;
		}
		if (activeFireTiles) {
			currentBulletHellCooldown += Time::GetDeltaTime();
		}

		if (dukeCharacter.lifePoints < lifeThreshold * dukeCharacter.GetTotalLifePoints()) {
			phase = Phase::PHASE2;
			phase2Reached = true;
			// Phase change VFX?
			lifeThreshold -= 10;
			break;
		}

		switch (dukeCharacter.state)
		{
		case DukeState::BASIC_BEHAVIOUR:
			if (dukeCharacter.lifePoints < 0.4 * dukeCharacter.GetTotalLifePoints()) {
				phase = Phase::PHASE3;
				// Phase change VFX?
				return;
			}

			if (currentBulletHellCooldown >= bulletHellCooldown) dukeCharacter.state = DukeState::BULLET_HELL;
			else if (phase2Reached) dukeCharacter.state = DukeState::CHARGE;
			else if (currentShieldCooldown >= shieldCooldown) dukeCharacter.state = DukeState::SHOOT_SHIELD;
			else if (player && movementScript->CharacterInAttackRange(player, dukeCharacter.attackRange)) dukeCharacter.state = DukeState::MELEE_ATTACK;
			else {
				// TODO: Compute new position
				dukeCharacter.ShootAndMove(ownerTransform->GetGlobalPosition());
			}
			break;
		case DukeState::MELEE_ATTACK:
			dukeCharacter.MeleeAttack();
			dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
			break;
		case DukeState::SHOOT_SHIELD:
			dukeCharacter.ShieldShoot();
			currentShieldActiveTime += Time::GetDeltaTime();
			if (currentShieldActiveTime >= shieldActiveTime) {
				currentShieldCooldown = 0.f;
				currentShieldActiveTime = 0.f;
				dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
			}
			break;
		case DukeState::BULLET_HELL:
			dukeCharacter.BulletHell();
			currentBulletHellActiveTime += Time::GetDeltaTime();
			if (currentBulletHellActiveTime >= bulletHellActiveTime) {
				currentBulletHellCooldown = 0.f;
				currentBulletHellActiveTime = 0.f;
				dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
			}
			break;
		case DukeState::CHARGE:
			dukeCharacter.Charge();
			dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
			break;
		default:
			break;
		}

	break;
	case Phase::PHASE2:
		Debug::Log("PHASE2");
		break;
	case Phase::PHASE3:
	default:
		break;
	}
}

void AIDuke::OnAnimationFinished()
{
}

void AIDuke::OnAnimationSecondaryFinished()
{
}

void AIDuke::OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle)
{
}
