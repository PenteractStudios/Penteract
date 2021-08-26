#include "AIDuke.h"

#include "AIMovement.h"
#include "PlayerController.h"
#include <string>

EXPOSE_MEMBERS(AIDuke) {
	MEMBER_SEPARATOR("Objects UIDs"),
    MEMBER(MemberType::GAME_OBJECT_UID, dukeUID),
    MEMBER(MemberType::GAME_OBJECT_UID, playerUID),

	MEMBER_SEPARATOR("Duke Atributes"),
	MEMBER(MemberType::FLOAT, dukeCharacter.lifePoints),
	MEMBER(MemberType::FLOAT, dukeCharacter.damageHit),
	MEMBER(MemberType::FLOAT, dukeCharacter.damageBullet),
	MEMBER(MemberType::FLOAT, dukeCharacter.damageCharge),
	MEMBER(MemberType::FLOAT, dukeCharacter.chargeSpeed),
	MEMBER(MemberType::FLOAT, dukeCharacter.movementSpeed),
	MEMBER(MemberType::FLOAT, dukeCharacter.searchRadius),
	MEMBER(MemberType::FLOAT, dukeCharacter.attackRange),
	MEMBER(MemberType::FLOAT, dukeCharacter.pushBackDistance),
	MEMBER(MemberType::FLOAT, dukeCharacter.pushBackSpeed),
	MEMBER(MemberType::FLOAT, dukeCharacter.moveChangeEvery),

	MEMBER_SEPARATOR("Duke Abilities Variables"),
	MEMBER(MemberType::FLOAT, shieldCooldown),
	MEMBER(MemberType::FLOAT, shieldActiveTime),
	MEMBER(MemberType::FLOAT, bulletHellCooldown),
    MEMBER(MemberType::FLOAT, bulletHellActiveTime),
	MEMBER(MemberType::FLOAT, abilityChangeCooldown),

	MEMBER_SEPARATOR("Particles UIDs"),

	MEMBER_SEPARATOR("Prefabs UIDs")
};

GENERATE_BODY_IMPL(AIDuke);

void AIDuke::Start() {
	duke = GameplaySystems::GetGameObject(dukeUID);
	player = GameplaySystems::GetGameObject(playerUID);
	if (player) {
		playerController = GET_SCRIPT(player, PlayerController);
	}
	movementScript = GET_SCRIPT(&GetOwner(), AIMovement);

	ownerTransform = GetOwner().GetComponent<ComponentTransform>();

	// Init Duke character
	dukeCharacter.Init(dukeUID, playerUID);
}

void AIDuke::Update() {
	std::string life = std::to_string(dukeCharacter.lifePoints);
	life = "Life points: " + life;
	Debug::Log(life.c_str());
	switch (phase) {
	case Phase::PHASE1:
		currentShieldCooldown += Time::GetDeltaTime();
		if ((dukeCharacter.lifePoints < 0.85 * dukeCharacter.GetTotalLifePoints()) && !activeFireTiles) {
			Debug::Log("BulletHell active and fire tiles on");
			activeFireTiles = true;
			// TODO: signal fire tiles activation
			currentBulletHellCooldown = 0.8 * bulletHellCooldown;
		}
		if (activeFireTiles) {
			currentBulletHellCooldown += Time::GetDeltaTime();
		}
		if (dukeCharacter.lifePoints < 0.4 * dukeCharacter.GetTotalLifePoints() &&
			dukeCharacter.state != DukeState::BULLET_HELL && dukeCharacter.state != DukeState::CHARGE) {
			phase = Phase::PHASE3;
			lifeThreshold -= 0.1;
			dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
			Debug::Log("Phase3");
			// Phase change VFX? and anim?
			return;
		} else if (dukeCharacter.lifePoints < lifeThreshold * dukeCharacter.GetTotalLifePoints() &&
			dukeCharacter.state != DukeState::BULLET_HELL && dukeCharacter.state != DukeState::CHARGE) {
			phase = Phase::PHASE2;
			if (!phase2Reached) phase2Reached = true;
			// Phase change VFX?
			// Anim + dissolve for teleportation
			lifeThreshold -= 0.1;
			activeFireTiles = false;
			Debug::Log("Fire tiles disabled");
			dukeCharacter.CallTroops();
			dukeCharacter.state = DukeState::INVULNERABLE;
			dukeCharacter.isShielding = false;
			dukeCharacter.criticalMode = true;
			break;
		}

		switch (dukeCharacter.state)
		{
		case DukeState::BASIC_BEHAVIOUR:
			if (currentBulletHellCooldown >= bulletHellCooldown) {
				dukeCharacter.state = DukeState::BULLET_HELL;
				movementScript->Stop();
			} else if (phase2Reached && playerController->playerOnimaru.shieldBeingUsed >= 2.0f) {
				dukeCharacter.chargeTarget = player->GetComponent<ComponentTransform>()->GetGlobalPosition();
				dukeCharacter.agent->SetMoveTarget(dukeCharacter.chargeTarget);
				dukeCharacter.agent->SetMaxSpeed(dukeCharacter.chargeSpeed);
				dukeCharacter.state = DukeState::CHARGE;
			} else if (currentShieldCooldown >= shieldCooldown) {
				dukeCharacter.state = DukeState::SHOOT_SHIELD;
				movementScript->Stop();
			} else if (player && movementScript->CharacterInAttackRange(player, dukeCharacter.attackRange)) {
				dukeCharacter.state = DukeState::MELEE_ATTACK;
				movementScript->Stop();
			} else {
				if (dukeCharacter.agent) dukeCharacter.agent->SetMaxSpeed(dukeCharacter.movementSpeed);
				float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition();
				movementScript->Orientate(dir);
				dukeCharacter.ShootAndMove(dir);
			}
			break;
		case DukeState::MELEE_ATTACK:
			dukeCharacter.MeleeAttack();
			dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
			break;
		case DukeState::SHOOT_SHIELD:
			movementScript->Orientate(player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition());
			dukeCharacter.ShieldShoot();
			currentShieldActiveTime += Time::GetDeltaTime();
			if (currentShieldActiveTime >= shieldActiveTime) {
				// TODO: Deactivate shield animation
				dukeCharacter.isShielding = false;
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
            dukeCharacter.Charge(DukeState::BASIC_BEHAVIOUR);
			break;
		default:
			break;
		}

	break;
	case Phase::PHASE2:
		Debug::Log("PHASE2");
		if (!activeLasers && dukeCharacter.lifePoints < lasersThreshold * dukeCharacter.GetTotalLifePoints()) {
			activeLasers = true;
			// TODO: signal lasers activation
			Debug::Log("Lasers enabled");
		}

		// TODO: Replace this for real enemy troops control
		if (troopsCounter <= 0) {
			troopsCounter = 5;
			activeFireTiles = true;
			Debug::Log("Fire tiles enabled");
			phase = Phase::PHASE1;
			dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
			currentBulletHellCooldown = 0.f;
			currentShieldCooldown = 0.f;
		}
		else {
			troopsCounter -= 0.033;
			float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition();
			movementScript->Orientate(dir);
			dukeCharacter.Shoot();
		}
		break;
	case Phase::PHASE3:
		if (dukeCharacter.lifePoints <= 0.f) {
			// TODO: Init victory sequence
			Debug::Log("Ugh...I'm...Dead...");
			GameplaySystems::DestroyGameObject(duke);
		}
		if (dukeCharacter.lifePoints < lifeThreshold * dukeCharacter.GetTotalLifePoints()) {
			dukeCharacter.criticalMode = !dukeCharacter.criticalMode;
			lifeThreshold -= 0.1f;
			if (!dukeCharacter.criticalMode) {
				dukeCharacter.CallTroops();
				dukeCharacter.state = DukeState::SHOOT_SHIELD;
				movementScript->Stop();
			}
		}
		if (dukeCharacter.state != DukeState::BULLET_HELL && player && !dukeCharacter.criticalMode &&
			movementScript->CharacterInAttackRange(player, dukeCharacter.attackRange)) {
			dukeCharacter.state = DukeState::MELEE_ATTACK;
			movementScript->Stop();
		}

		if (dukeCharacter.criticalMode) {
			switch (dukeCharacter.state) {
			case DukeState::BASIC_BEHAVIOUR:
				currentAbilityChangeCooldown += Time::GetDeltaTime();
				if (currentAbilityChangeCooldown >= abilityChangeCooldown) {
					currentAbilityChangeCooldown = 0.f;
					dukeCharacter.chargeTarget = player->GetComponent<ComponentTransform>()->GetGlobalPosition();
					dukeCharacter.agent->SetMoveTarget(dukeCharacter.chargeTarget);
					dukeCharacter.agent->SetMaxSpeed(dukeCharacter.chargeSpeed);
					dukeCharacter.state = DukeState::CHARGE;
				}
				else {
					if (dukeCharacter.agent) dukeCharacter.agent->SetMaxSpeed(dukeCharacter.movementSpeed);
					float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition();
					movementScript->Orientate(dir);
					dukeCharacter.ShootAndMove(dir);
				}
				break;
			case DukeState::CHARGE:
				dukeCharacter.Charge(DukeState::MELEE_ATTACK);
				break;
			case DukeState::MELEE_ATTACK:
				dukeCharacter.MeleeAttack();
				dukeCharacter.state = DukeState::BULLET_HELL;
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
			default:
				break;
			}
		} else {
			switch (dukeCharacter.state)
			{
			case DukeState::SHOOT_SHIELD:
				movementScript->Orientate(player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition());
				dukeCharacter.ShieldShoot();
				currentAbilityChangeCooldown += Time::GetDeltaTime();
				if (currentAbilityChangeCooldown >= abilityChangeCooldown) {
					// TODO: Deactivate shield animation
					dukeCharacter.isShielding = false;
					currentAbilityChangeCooldown = 0.f;
					dukeCharacter.state = DukeState::BULLET_HELL;
				}
				break;
			case DukeState::BULLET_HELL:
				dukeCharacter.BulletHell();
				currentAbilityChangeCooldown += Time::GetDeltaTime();
				if (currentAbilityChangeCooldown >= abilityChangeCooldown) {
					currentAbilityChangeCooldown = 0.f;
					dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
				}
				break;
			case DukeState::BASIC_BEHAVIOUR:
				currentAbilityChangeCooldown += Time::GetDeltaTime();
				if (currentAbilityChangeCooldown >= abilityChangeCooldown) {
					currentAbilityChangeCooldown = 0.f;
					dukeCharacter.state = DukeState::SHOOT_SHIELD;
					movementScript->Stop();
				} else {
					if (dukeCharacter.agent) dukeCharacter.agent->SetMaxSpeed(dukeCharacter.movementSpeed);
					float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition();
					movementScript->Orientate(dir);
					dukeCharacter.ShootAndMove(dir);
				}
				break;
			case DukeState::MELEE_ATTACK:
				dukeCharacter.MeleeAttack();
				dukeCharacter.state = DukeState::SHOOT_SHIELD;
				break;
			default:
				break;
			}
		}
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
	if (dukeCharacter.isAlive && playerController) {
		bool hitTaken = false;
		if (collidedWith.name == "FangBullet") {
			if (!particle) return;
			GameplaySystems::DestroyGameObject(&collidedWith);
			hitTaken = true;
			dukeCharacter.GetHit(playerController->playerFang.damageHit + playerController->GetOverPowerMode());
		}
		else if (collidedWith.name == "OnimaruBullet") {
			if (!particle) return;
			ComponentParticleSystem::Particle* p = (ComponentParticleSystem::Particle*)particle;
			ComponentParticleSystem* pSystem = collidedWith.GetComponent<ComponentParticleSystem>();
			if (pSystem) pSystem->KillParticle(p);
			hitTaken = true;
			dukeCharacter.GetHit(playerController->playerOnimaru.damageHit + playerController->GetOverPowerMode());
		}
		else if (collidedWith.name == "OnimaruBulletUltimate") {
			if (!particle) return;
			ComponentParticleSystem::Particle* p = (ComponentParticleSystem::Particle*)particle;
			ComponentParticleSystem* pSystem = collidedWith.GetComponent<ComponentParticleSystem>();
			if (pSystem) pSystem->KillParticle(p);
			hitTaken = true;
			dukeCharacter.GetHit(playerController->playerOnimaru.damageHit + playerController->GetOverPowerMode());
		}
		else if (collidedWith.name == "Barrel") {
			hitTaken = true;
			dukeCharacter.GetHit(dukeCharacter.barrelDamageTaken);
		}

		if (hitTaken) {
			/*if (audios[static_cast<int>(AudioType::HIT)]) audios[static_cast<int>(AudioType::HIT)]->Play();
			if (componentMeshRenderer) {
				if (damageMaterialID != 0) componentMeshRenderer->materialId = damageMaterialID;
			}

			timeSinceLastHurt = 0.0f;*/
		}

		if (collidedWith.name == "EMP") {
			/*if (state == AIState::ATTACK) {
				animation->SendTrigger("IdleBeginStun");
				animation->SendTriggerSecondary("AttackBeginStun");
			}
			else if (state == AIState::IDLE) {
				animation->SendTrigger("IdleBeginStun");
			}
			else if (state == AIState::RUN) {
				animation->SendTrigger("RunBeginStun");
			}
			agent->RemoveAgentFromCrowd();
			*/
			stunTimeRemaining = stunDuration;
			dukeCharacter.state = DukeState::STUNNED;
		}
	}

	if (!dukeCharacter.isAlive) {
		/*std::string curState = "Run";
		if (animation->GetCurrentState()->name == "Idle") curState = "Idle";
		DeleteAttackCollider();
		deathType = (rand() % 2 == 0) ? true : false;
		if (state == AIState::ATTACK) {
			if (deathType) {
				animation->SendTrigger(curState + "Death1");
				animation->SendTriggerSecondary("AttackDeath1");
			}
			else {
				animation->SendTrigger(curState + "Death2");
				animation->SendTriggerSecondary("AttackDeath2");
			}
		}
		else if (state == AIState::IDLE) {
			if (deathType) {
				animation->SendTrigger("IdleDeath1");
			}
			else {
				animation->SendTrigger("IdleDeath1");
			}
		}
		else if (state == AIState::RUN) {
			if (deathType) {
				animation->SendTrigger("RunDeath1");
			}
			else {
				animation->SendTrigger("RunDeath2");
			}
		}
		else if (state == AIState::STUNNED) {
			if (deathType) {
				animation->SendTrigger("StunnedDeath1");
			}
			else {
				animation->SendTrigger("StunnedDeath2");
			}
		}
		else if (state == AIState::PUSHED) {
			if (deathType) {
				animation->SendTrigger("HurtDeath1");
			}
			else {
				animation->SendTrigger("HurtDeath2");
			}
		}

		if (audios[static_cast<int>(AudioType::DEATH)]) audios[static_cast<int>(AudioType::DEATH)]->Play();
		ComponentCapsuleCollider* collider = GetOwner().GetComponent<ComponentCapsuleCollider>();
		if (collider) collider->Disable();

		agent->RemoveAgentFromCrowd();
		if (gruntCharacter.beingPushed) gruntCharacter.beingPushed = false;*/
		dukeCharacter.state = DukeState::DEATH;
	}
}
