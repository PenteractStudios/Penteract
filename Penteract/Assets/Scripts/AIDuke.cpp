#include "AIDuke.h"

#include "AIMovement.h"
#include "PlayerController.h"
#include "DukeShield.h"
#include <string>
#include <vector>

EXPOSE_MEMBERS(AIDuke) {
	MEMBER_SEPARATOR("Objects UIDs"),
	MEMBER(MemberType::GAME_OBJECT_UID, dukeUID),
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, shieldObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, bulletUID),
	MEMBER(MemberType::GAME_OBJECT_UID, chargeColliderUID),
	MEMBER(MemberType::GAME_OBJECT_UID, meleeAttackColliderUID),
	MEMBER(MemberType::GAME_OBJECT_UID, chargeAttackUID),
	MEMBER(MemberType::GAME_OBJECT_UID, chargeColliderUID),
	MEMBER(MemberType::GAME_OBJECT_UID, firstEncounterUID),
	MEMBER(MemberType::GAME_OBJECT_UID, secondEncounterUID),
	MEMBER(MemberType::GAME_OBJECT_UID, thirdEncounterUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fourthEncounterUID),

	MEMBER_SEPARATOR("Duke Atributes"),
	MEMBER(MemberType::FLOAT, dukeCharacter.lifePoints),
	MEMBER(MemberType::FLOAT, dukeCharacter.chargeSpeed),
	MEMBER(MemberType::FLOAT, dukeCharacter.chargeMinimumDistance),
	MEMBER(MemberType::FLOAT, dukeCharacter.movementSpeed),
	MEMBER(MemberType::FLOAT, dukeCharacter.searchRadius),
	MEMBER(MemberType::FLOAT, dukeCharacter.attackRange),
	MEMBER(MemberType::FLOAT, dukeCharacter.attackSpeed),
	MEMBER(MemberType::INT, dukeCharacter.attackBurst),
	MEMBER(MemberType::FLOAT, dukeCharacter.timeInterBurst),
	MEMBER(MemberType::FLOAT, dukeCharacter.pushBackDistance),
	MEMBER(MemberType::FLOAT, dukeCharacter.pushBackSpeed),
	MEMBER(MemberType::FLOAT, dukeCharacter.slowedDownTime),
	MEMBER(MemberType::FLOAT, dukeCharacter.slowedDownSpeed),
	MEMBER(MemberType::FLOAT, dukeCharacter.moveChangeEvery),
	MEMBER(MemberType::FLOAT, dukeCharacter.distanceCorrectEvery),

	MEMBER_SEPARATOR("Duke Abilities Variables"),
	MEMBER(MemberType::FLOAT, shieldCooldown),
	MEMBER(MemberType::FLOAT, shieldActiveTime),
	MEMBER(MemberType::FLOAT, bulletHellCooldown),
	MEMBER(MemberType::FLOAT, bulletHellActiveTime),
	MEMBER(MemberType::FLOAT, abilityChangeCooldown),
	MEMBER(MemberType::FLOAT, throwBarrelTimer),
	MEMBER(MemberType::FLOAT, orientationSpeed),
	MEMBER(MemberType::FLOAT, orientationThreshold),

	MEMBER_SEPARATOR("Particles UIDs"),

	MEMBER_SEPARATOR("Prefabs UIDs"),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, barrelUID),

	MEMBER_SEPARATOR("Debug"),
	MEMBER(MemberType::BOOL, toggleShield),
	MEMBER(MemberType::SCENE_RESOURCE_UID, winSceneUID),


};

GENERATE_BODY_IMPL(AIDuke);

void AIDuke::Start() {
	duke = GameplaySystems::GetGameObject(dukeUID);
	player = GameplaySystems::GetGameObject(playerUID);
	if (player) {
		playerController = GET_SCRIPT(player, PlayerController);
		if (playerController) playerController->AddEnemyInMap(duke);
	}
	movementScript = GET_SCRIPT(&GetOwner(), AIMovement);

	ownerTransform = GetOwner().GetComponent<ComponentTransform>();

	/* Encounters */
	std::vector<UID> encounters;
	encounters.push_back(firstEncounterUID);
	encounters.push_back(secondEncounterUID);
	encounters.push_back(thirdEncounterUID);
	encounters.push_back(fourthEncounterUID);

	// Debug
	GameObject* shieldObj = GameplaySystems::GetGameObject(shieldObjUID);
	if (shieldObj) {
		dukeShield = GET_SCRIPT(shieldObj, DukeShield);
	}

	// Init Duke character
	dukeCharacter.Init(dukeUID, playerUID, bulletUID, barrelUID, chargeColliderUID, meleeAttackColliderUID, chargeAttackUID, encounters);

	dukeCharacter.winSceneUID = winSceneUID; // TODO: REPLACE
}

void AIDuke::Update() {
	std::string life = std::to_string(dukeCharacter.lifePoints);
	life = "Life points: " + life;
	Debug::Log(life.c_str());

	if (toggleShield) {
		toggleShield = false;
		if (dukeShield) {
			if (!dukeShield->GetIsActive()) {
				dukeShield->InitShield();
			} else {
				dukeShield->FadeShield();
			}
		}
	}

	float speedToUse = dukeCharacter.slowedDown ? dukeCharacter.slowedDownSpeed : dukeCharacter.movementSpeed;

	if (dukeCharacter.slowedDown) {
		if (currentSlowedDownTime >= dukeCharacter.slowedDownTime) {
			dukeCharacter.agent->SetMaxSpeed(dukeCharacter.movementSpeed);
			dukeCharacter.slowedDown = false;
		}
		currentSlowedDownTime += Time::GetDeltaTime();
	}

	switch (phase) {
	case Phase::PHASE1:
		currentShieldCooldown += Time::GetDeltaTime();
		if ((dukeCharacter.lifePoints < 0.85 * dukeCharacter.GetTotalLifePoints()) && !activeFireTiles) {
			Debug::Log("BulletHell active and fire tiles on");
			activeFireTiles = true;
			// TODO: signal fire tiles activation
			currentBulletHellCooldown = 0.8f * bulletHellCooldown;
		}
		if (activeFireTiles) {
			currentBulletHellCooldown += Time::GetDeltaTime();
		}
		if (dukeCharacter.lifePoints < 0.4 * dukeCharacter.GetTotalLifePoints() &&
			dukeCharacter.state != DukeState::BULLET_HELL && dukeCharacter.state != DukeState::CHARGE) {
			phase = Phase::PHASE3;
			lifeThreshold -= 0.1f;
			Debug::Log("Phase3");
			dukeCharacter.criticalMode = true;
			// Phase change VFX? and anim?
			movementScript->Stop();
			if (dukeShield && dukeShield->GetIsActive()) dukeShield->FadeShield();
			if (dukeCharacter.compAnimation) {
				dukeCharacter.StopShooting();
				dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::ENRAGE]);
			}
			dukeCharacter.state = DukeState::INVULNERABLE;
			return;
		} else if (dukeCharacter.lifePoints < lifeThreshold * dukeCharacter.GetTotalLifePoints() &&
				 dukeCharacter.state != DukeState::BULLET_HELL && dukeCharacter.state != DukeState::CHARGE) {
			phase = Phase::PHASE2;
			if (!phase2Reached) phase2Reached = true;
			// Phase change VFX?
			// Anim + dissolve for teleportation
			lifeThreshold -= 0.15f;
			activeFireTiles = false;
			Debug::Log("Fire tiles disabled");
			movementScript->Stop();
			if (isInArena) TeleportDuke(true);
			dukeCharacter.CallTroops();
			dukeCharacter.state = DukeState::INVULNERABLE;
			if (dukeShield && dukeShield->GetIsActive()) dukeShield->FadeShield();
			dukeCharacter.StopShooting();
			break;
		}

		switch (dukeCharacter.state) {
		case DukeState::BASIC_BEHAVIOUR:
			if (currentBulletHellCooldown >= bulletHellCooldown) {
				dukeCharacter.state = DukeState::BULLET_HELL;
				movementScript->Stop();
				// TODO: Delete next line
				dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::IDLE]);
			} else if (phase2Reached && playerController->playerOnimaru.shieldBeingUsed >= 2.0f) {
				// If onimaru shields -> perform charge
				movementScript->Stop();
				dukeCharacter.InitCharge(DukeState::BASIC_BEHAVIOUR);
			} else if (currentShieldCooldown >= shieldCooldown) {
				if (dukeShield) dukeShield->InitShield();
				dukeCharacter.state = DukeState::SHOOT_SHIELD;
				movementScript->Stop();
				// TODO: Delete next line
				dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::IDLE] );
			} else if (player && movementScript->CharacterInAttackRange(player, dukeCharacter.attackRange)) {
				// If player too close -> perform melee attack
				dukeCharacter.state = DukeState::MELEE_ATTACK;
				movementScript->Stop();
			} else {
				if (player) {
					if ((float3(0, 0, 0) - player->GetComponent<ComponentTransform>()->GetGlobalPosition()).LengthSq() <
						(float3(0, 0, 0) - ownerTransform->GetGlobalPosition()).LengthSq()) {
						// If player dominates the center for too long, perform charge
						timeSinceLastCharge += Time::GetDeltaTime();
					}
					if (timeSinceLastCharge >= 3.0f) {
						timeSinceLastCharge = 0.f;
						// Charge
						movementScript->Stop();
						dukeCharacter.InitCharge(DukeState::BASIC_BEHAVIOUR);
						dukeCharacter.StopShooting();
					} else {
						// Normal behavior
						if (dukeCharacter.agent) dukeCharacter.agent->SetMaxSpeed(speedToUse);
						float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition();
						dir.y = 0.0f;
						movementScript->Orientate(dir);
						dukeCharacter.ShootAndMove(dir);
					}
				}
				break;
			}
			dukeCharacter.StopShooting();

			break;
		case DukeState::MELEE_ATTACK:
			dukeCharacter.MeleeAttack();
			break;
		case DukeState::SHOOT_SHIELD:
			if (player) movementScript->Orientate(player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition(), orientationSpeed, orientationThreshold);
			dukeCharacter.Shoot();
			currentShieldActiveTime += Time::GetDeltaTime();
			if (currentShieldActiveTime >= shieldActiveTime) {
				// TODO: Deactivate shield animation
				if (dukeShield) dukeShield->FadeShield();
				currentShieldCooldown = 0.f;
				currentShieldActiveTime = 0.f;
				dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;

				dukeCharacter.StopShooting();
			}
			break;
		case DukeState::BULLET_HELL:
			dukeCharacter.reducedDamaged = true;
			dukeCharacter.BulletHell();
			currentBulletHellActiveTime += Time::GetDeltaTime();
			if (currentBulletHellActiveTime >= bulletHellActiveTime) {
				dukeCharacter.reducedDamaged = false;
				currentBulletHellCooldown = 0.f;
				currentBulletHellActiveTime = 0.f;
				dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
			}
			break;
		case DukeState::CHARGE:
			dukeCharacter.UpdateCharge();
			break;
		case DukeState::STUNNED:
			if (stunTimeRemaining <= 0.f) {
				stunTimeRemaining = 0.f;
				dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
			} else {
				stunTimeRemaining -= Time::GetDeltaTime();
			}
			break;
		case DukeState::PUSHED:
			UpdatePushBackPosition();
			break;
		default:
			break;
		}

		break;
	case Phase::PHASE2:
		if (!activeLasers && dukeCharacter.lifePoints < lasersThreshold * dukeCharacter.GetTotalLifePoints()) {
			activeLasers = true;
			// TODO: signal lasers activation
			Debug::Log("Lasers enabled");
		}

		if (isInArena) {
			activeFireTiles = true;
			Debug::Log("Fire tiles enabled");
			phase = Phase::PHASE1;
			dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
			currentBulletHellCooldown = 0.f;
			currentShieldCooldown = 0.f;
		} else {
			if (player) {
				float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition();
				movementScript->Orientate(dir);
			}
			currentBarrelTimer += Time::GetDeltaTime();
			if (currentBarrelTimer >= throwBarrelTimer) {
				dukeCharacter.ThrowBarrels();
				currentBarrelTimer = 0.f;
			}
		}
		break;
	case Phase::PHASE3:
		if (dukeCharacter.lifePoints <= 0.f) {
			// TODO: Init victory sequence
			Debug::Log("Ugh...I'm...Dead...");
			if (playerController) playerController->RemoveEnemyFromMap(duke);
			return;

		} else if (dukeCharacter.lifePoints < lifeThreshold * dukeCharacter.GetTotalLifePoints()) {
			dukeCharacter.criticalMode = !dukeCharacter.criticalMode;
			lifeThreshold -= 0.1f;
			if (!dukeCharacter.criticalMode) {
				movementScript->Stop();
				if (dukeCharacter.compAnimation) {
					dukeCharacter.StopShooting();
					dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::STUN]);
				}
				dukeCharacter.state = DukeState::INVULNERABLE;


			} else {
				movementScript->Stop();
				if (dukeShield && dukeShield->GetIsActive()) dukeShield->FadeShield();
				if (dukeCharacter.compAnimation) {
					dukeCharacter.StopShooting();
					dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::ENRAGE]);
				}
				dukeCharacter.state = DukeState::INVULNERABLE;
			}
		}
		if (dukeCharacter.state != DukeState::BULLET_HELL && dukeCharacter.state != DukeState::STUNNED &&
			player && !dukeCharacter.criticalMode &&
			movementScript->CharacterInAttackRange(player, dukeCharacter.attackRange)) {
			dukeCharacter.state = DukeState::MELEE_ATTACK;
			movementScript->Stop();

			dukeCharacter.StopShooting();
		}

		if (dukeCharacter.criticalMode) {
			switch (dukeCharacter.state) {
			case DukeState::BASIC_BEHAVIOUR:
				currentAbilityChangeCooldown += Time::GetDeltaTime();
				if (currentAbilityChangeCooldown >= abilityChangeCooldown) {
					currentAbilityChangeCooldown = 0.f;
					movementScript->Stop();
					dukeCharacter.InitCharge(DukeState::MELEE_ATTACK);

					dukeCharacter.StopShooting();
				} else {
					if (dukeCharacter.agent) dukeCharacter.agent->SetMaxSpeed(speedToUse);
					float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition();
					movementScript->Orientate(dir);
					dukeCharacter.ShootAndMove(dir);
				}
				break;
			case DukeState::CHARGE:
				dukeCharacter.UpdateCharge();
				break;
			case DukeState::MELEE_ATTACK:
				dukeCharacter.MeleeAttack();
				dukeCharacter.state = DukeState::BULLET_HELL;
				break;
			case DukeState::BULLET_HELL:
				dukeCharacter.reducedDamaged = true;
				dukeCharacter.BulletHell();
				currentBulletHellActiveTime += Time::GetDeltaTime();
				if (currentBulletHellActiveTime >= bulletHellActiveTime) {
					dukeCharacter.reducedDamaged = false;
					currentBulletHellCooldown = 0.f;
					currentBulletHellActiveTime = 0.f;
					dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
				}
				break;
			case DukeState::STUNNED:
				if (stunTimeRemaining <= 0.f) {
					stunTimeRemaining = 0.f;
					dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
					//animation->SendTrigger("StunStunEnd");
				} else {
					stunTimeRemaining -= Time::GetDeltaTime();
				}
				break;
			case DukeState::PUSHED:
				UpdatePushBackPosition();
				break;
			default:
				break;
			}
		} else {
			switch (dukeCharacter.state) {
			case DukeState::SHOOT_SHIELD:
				if (dukeShield && !dukeShield->GetIsActive()) dukeShield->InitShield();
				movementScript->Orientate(player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition(), orientationSpeed, orientationThreshold);
				dukeCharacter.Shoot();
				currentAbilityChangeCooldown += Time::GetDeltaTime();
				if (currentAbilityChangeCooldown >= abilityChangeCooldown) {
					// TODO: Deactivate shield animation
					if (dukeShield) dukeShield->FadeShield();
					currentAbilityChangeCooldown = 0.f;
					dukeCharacter.state = DukeState::BULLET_HELL;

					dukeCharacter.StopShooting();
				}
				break;
			case DukeState::BULLET_HELL:
				dukeCharacter.reducedDamaged = true;
				dukeCharacter.BulletHell();
				currentAbilityChangeCooldown += Time::GetDeltaTime();
				if (currentAbilityChangeCooldown >= abilityChangeCooldown) {
					dukeCharacter.reducedDamaged = false;
					currentAbilityChangeCooldown = 0.f;
					dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
				}
				break;
			case DukeState::BASIC_BEHAVIOUR:
				currentAbilityChangeCooldown += Time::GetDeltaTime();
				if (currentAbilityChangeCooldown >= abilityChangeCooldown) {
					currentAbilityChangeCooldown = 0.f;
					if (dukeShield) dukeShield->InitShield();
					dukeCharacter.state = DukeState::SHOOT_SHIELD;
					movementScript->Stop();
					// TODO: Delete next line
					dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::IDLE]);
				}
				else {
					if (player) {
						if ((float3(0, 0, 0) - player->GetComponent<ComponentTransform>()->GetGlobalPosition()).LengthSq() <
							(float3(0, 0, 0) - ownerTransform->GetGlobalPosition()).LengthSq()) {
							// If player dominates the center for too long, perform charge
							timeSinceLastCharge += Time::GetDeltaTime();
						}
						if (timeSinceLastCharge >= 4.0f) {
							timeSinceLastCharge = 0.f;
							// Charge
							movementScript->Stop();
							dukeCharacter.InitCharge(DukeState::BASIC_BEHAVIOUR);

							dukeCharacter.StopShooting();
						}
						else {
							// Normal behavior
							if (dukeCharacter.agent) dukeCharacter.agent->SetMaxSpeed(speedToUse);
							float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition();
							dir.y = 0.0f;
							movementScript->Orientate(dir);
							dukeCharacter.ShootAndMove(dir);
						}
					}
				}
				break;
			case DukeState::MELEE_ATTACK:
				dukeCharacter.MeleeAttack();
				if (dukeShield) dukeShield->InitShield();
				dukeCharacter.state = DukeState::SHOOT_SHIELD;
				break;
			case DukeState::STUNNED:
				if (stunTimeRemaining <= 0.f) {
					stunTimeRemaining = 0.f;
					dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
					//animation->SendTrigger("StunStunEnd");
				} else {
					stunTimeRemaining -= Time::GetDeltaTime();
				}
				break;
			case DukeState::PUSHED:
				UpdatePushBackPosition();
				break;
			default:
				break;
			}
		}
	default:
		break;
	}
}

void AIDuke::OnAnimationFinished() {
	dukeCharacter.OnAnimationFinished();
}

void AIDuke::OnAnimationSecondaryFinished() {
	dukeCharacter.OnAnimationSecondaryFinished();
}

void AIDuke::OnAnimationEvent(StateMachineEnum stateMachineEnum, const char* eventName) {
	dukeCharacter.OnAnimationEvent(stateMachineEnum, eventName);
}

void AIDuke::OnCollision(GameObject& collidedWith, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* particle) {
	if (dukeCharacter.isAlive && playerController) {
		bool hitTaken = false;
		if (dukeCharacter.state != DukeState::INVULNERABLE) {
			if (collidedWith.name == "FangBullet") {
				if (!particle) return;
				GameplaySystems::DestroyGameObject(&collidedWith);
				hitTaken = true;
				float damage = playerController->playerFang.damageHit;
				dukeCharacter.GetHit( dukeCharacter.reducedDamaged ? damage / 3 : damage + playerController->GetOverPowerMode());
			}
			else if (collidedWith.name == "FangRightBullet" || collidedWith.name == "FangLeftBullet") {
				hitTaken = true;
				ParticleHit(collidedWith, particle, playerController->playerFang);
			}
			else if (collidedWith.name == "OnimaruBullet") {
				hitTaken = true;
				ParticleHit(collidedWith, particle, playerController->playerOnimaru);
			}
			else if (collidedWith.name == "OnimaruBulletUltimate") {
				hitTaken = true;
				ParticleHit(collidedWith, particle, playerController->playerOnimaru);
			}
			else if (collidedWith.name == "DashDamage" && playerController->playerFang.level1Upgrade) {
				hitTaken = true;
				float damage = playerController->playerFang.dashDamage;
				dukeCharacter.GetHit(dukeCharacter.reducedDamaged ? damage / 3 : damage + playerController->GetOverPowerMode());
			}

			if (hitTaken) {
				// TODO: play audio and VFX
				/*if (audios[static_cast<int>(AudioType::HIT)]) audios[static_cast<int>(AudioType::HIT)]->Play();
				if (componentMeshRenderer) {
					if (damageMaterialID != 0) componentMeshRenderer->materialId = damageMaterialID;
				}

				timeSinceLastHurt = 0.0f;*/
			}
		}

		if (collidedWith.name == "EMP" && dukeCharacter.state != DukeState::INVULNERABLE && dukeCharacter.state != DukeState::CHARGE) {
			dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::STUN]);
			dukeCharacter.StopShooting();
			movementScript->Stop();
			stunTimeRemaining = stunDuration;
			dukeCharacter.state = DukeState::STUNNED;

			dukeCharacter.StopShooting();
		}
	}

	if (!dukeCharacter.isAlive) {
		movementScript->Stop();
		dukeCharacter.StopShooting();
		dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::DEATH]);

		//if (audios[static_cast<int>(AudioType::DEATH)]) audios[static_cast<int>(AudioType::DEATH)]->Play();
		ComponentCapsuleCollider* collider = GetOwner().GetComponent<ComponentCapsuleCollider>();
		if (collider) collider->Disable();

		dukeCharacter.agent->RemoveAgentFromCrowd();
		if (dukeCharacter.beingPushed) dukeCharacter.beingPushed = false;
		dukeCharacter.state = DukeState::DEATH;
	}
}

void AIDuke::EnableBlastPushBack() {
	if (dukeCharacter.state != DukeState::INVULNERABLE && dukeCharacter.state != DukeState::CHARGE) {
		dukeCharacter.beingPushed = true;
		dukeCharacter.state = DukeState::PUSHED;
		dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::PUSHED]);
		dukeCharacter.StopShooting();
		CalculatePushBackRealDistance();
		// Damage
		if (playerController->playerOnimaru.level2Upgrade) {
			dukeCharacter.GetHit(playerController->playerOnimaru.blastDamage + playerController->GetOverPowerMode());

			//if (audios[static_cast<int>(AudioType::HIT)]) audios[static_cast<int>(AudioType::HIT)]->Play();
			//PlayHitMaterialEffect();
			//timeSinceLastHurt = 0.0f;
		}
	}
}

void AIDuke::DisableBlastPushBack() {
	if (dukeCharacter.state != DukeState::INVULNERABLE) {
		dukeCharacter.beingPushed = false;
		//if (animation->GetCurrentState()) animation->SendTrigger(animation->GetCurrentState()->name + "Idle");
		dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
	}
}

bool AIDuke::IsBeingPushed() const {
	return dukeCharacter.beingPushed;
}

void AIDuke::CalculatePushBackRealDistance() {
	float3 playerPos = player->GetComponent<ComponentTransform>()->GetGlobalPosition();
	float3 enemyPos = GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition();

	float3 direction = (enemyPos - playerPos).Normalized();

	bool hitResult = false;

	float3 finalPos = enemyPos + direction * dukeCharacter.pushBackDistance;
	float3 resultPos = { 0,0,0 };

	Navigation::Raycast(enemyPos, finalPos, hitResult, resultPos);

	if (hitResult) {
		pushBackRealDistance = resultPos.Distance(enemyPos) - 1; // Should be agent radius but it's not exposed
	}
}

void AIDuke::UpdatePushBackPosition() {
	float3 playerPos = player->GetComponent<ComponentTransform>()->GetGlobalPosition();
	float3 enemyPos = GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition();
	float3 initialPos = enemyPos;

	float3 direction = (enemyPos - playerPos).Normalized();

	if (dukeCharacter.agent) {
		enemyPos += direction * dukeCharacter.pushBackSpeed * Time::GetDeltaTime();
		dukeCharacter.agent->SetMoveTarget(enemyPos, false);
		dukeCharacter.agent->SetMaxSpeed(dukeCharacter.pushBackSpeed);
		float distance = enemyPos.Distance(initialPos);
		currentPushBackDistance += distance;

		if (currentPushBackDistance >= pushBackRealDistance) {
			dukeCharacter.agent->SetMaxSpeed(dukeCharacter.slowedDownSpeed);
			DisableBlastPushBack();
			dukeCharacter.slowedDown = true;
			currentPushBackDistance = 0.f;
			currentSlowedDownTime = 0.f;
			pushBackRealDistance = dukeCharacter.pushBackDistance;
		}
	}
}

void AIDuke::ParticleHit(GameObject& collidedWith, void* particle, Player& player) {
	if (!particle) return;
	ComponentParticleSystem::Particle* p = (ComponentParticleSystem::Particle*)particle;
	ComponentParticleSystem* pSystem = collidedWith.GetComponent<ComponentParticleSystem>();
	if (pSystem) pSystem->KillParticle(p);
	float damage = dukeCharacter.reducedDamaged ? player.damageHit / 3 : player.damageHit;
	if (dukeCharacter.state == DukeState::STUNNED && player.level2Upgrade) {
		dukeCharacter.GetHit(damage * 2 + playerController->GetOverPowerMode());
	} else {
		dukeCharacter.GetHit(damage + playerController->GetOverPowerMode());
	}
}

void AIDuke::TeleportDuke(bool toPlatform) {
	if (toPlatform) {
		if (dukeCharacter.agent) dukeCharacter.agent->RemoveAgentFromCrowd();
		ownerTransform->SetGlobalPosition(float3(40.0f, 0.0f, 0.0f));
		isInArena = false;
	} else {
		ownerTransform->SetGlobalPosition(float3(0.0f, 0.0f, 0.0f));
		if (dukeCharacter.agent) dukeCharacter.agent->AddAgentToCrowd();
		isInArena = true;
	}
}