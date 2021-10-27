#include "AIDuke.h"

#include "AIMovement.h"
#include "PlayerController.h"
#include "DukeShield.h"
#include "GlobalVariables.h"
#include "HUDManager.h"
#include "AttackDronesController.h"
#include "FloorIsLava.h"
#include <string>
#include <vector>

EXPOSE_MEMBERS(AIDuke) {
	MEMBER_SEPARATOR("Objects UIDs"),
	MEMBER(MemberType::GAME_OBJECT_UID, dukeUID),
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, shieldObjUID),
	MEMBER(MemberType::GAME_OBJECT_UID, bulletUID),
	MEMBER(MemberType::GAME_OBJECT_UID, meleeAttackColliderUID),
	MEMBER(MemberType::GAME_OBJECT_UID, barrelSpawnerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, chargeAttackUID),
	MEMBER(MemberType::GAME_OBJECT_UID, lasersUID),
	MEMBER(MemberType::GAME_OBJECT_UID, chargeColliderUID),
	MEMBER(MemberType::GAME_OBJECT_UID, phase2ShieldUID),
	MEMBER(MemberType::GAME_OBJECT_UID, firstEncounterUID),
	MEMBER(MemberType::GAME_OBJECT_UID, secondEncounterUID),
	MEMBER(MemberType::GAME_OBJECT_UID, thirdEncounterUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fourthEncounterUID),
	MEMBER(MemberType::GAME_OBJECT_UID, hudManagerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, fireTilesUID),
	MEMBER(MemberType::GAME_OBJECT_UID, triggerBossEndUID),

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
	MEMBER(MemberType::FLOAT, dukeCharacter.pushBackTime),
	MEMBER(MemberType::FLOAT, dukeCharacter.slowedDownTime),
	MEMBER(MemberType::FLOAT, dukeCharacter.slowedDownSpeed),
	MEMBER(MemberType::FLOAT, dukeCharacter.moveChangeEvery),
	MEMBER(MemberType::FLOAT, dukeCharacter.distanceCorrectEvery),
	MEMBER(MemberType::BOOL, dukeCharacter.startSpawnBarrel),

	MEMBER_SEPARATOR("Duke Abilities Variables"),
	MEMBER(MemberType::FLOAT, shieldCooldown),
	MEMBER(MemberType::FLOAT, shieldActiveTime),
	MEMBER(MemberType::FLOAT, bulletHellCooldown),
	MEMBER(MemberType::FLOAT, abilityChangeCooldown),
	MEMBER(MemberType::FLOAT, criticalModeCooldown),
	MEMBER(MemberType::FLOAT, throwBarrelTimer),
	MEMBER(MemberType::FLOAT, orientationSpeed),
	MEMBER(MemberType::FLOAT, orientationThreshold),
	MEMBER(MemberType::FLOAT, orientationSpeedBulletHell),
	MEMBER(MemberType::FLOAT, orientationThresholdBulletHell),
	MEMBER(MemberType::FLOAT, timerBetweenAbilities),

	MEMBER_SEPARATOR("Particles UIDs"),
	MEMBER(MemberType::GAME_OBJECT_UID, punchSlashUID),
	MEMBER(MemberType::GAME_OBJECT_UID, chargeDustUID),
	MEMBER(MemberType::GAME_OBJECT_UID, areaChargeUID),
	MEMBER(MemberType::GAME_OBJECT_UID, chargeTelegraphAreaUID),

	MEMBER_SEPARATOR("Prefabs UIDs"),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, barrelUID),

	MEMBER_SEPARATOR("Charge Skid"),
	MEMBER(MemberType::FLOAT, dukeCharacter.chargeSkidDuration),
	MEMBER(MemberType::FLOAT, dukeCharacter.chargeSkidMaxSpeed),
	MEMBER(MemberType::FLOAT, dukeCharacter.chargeSkidMinSpeed),


	MEMBER_SEPARATOR("Debug"),
	MEMBER(MemberType::SCENE_RESOURCE_UID, winSceneUID),
	MEMBER(MemberType::BOOL, islevel2),

	MEMBER_SEPARATOR("Dissolve material reference in placeholders"),
	MEMBER(MemberType::GAME_OBJECT_UID, dissolveMaterialGOUID)

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

	// Encounters
	std::vector<UID> encounters;
	encounters.push_back(firstEncounterUID);
	encounters.push_back(secondEncounterUID);
	encounters.push_back(thirdEncounterUID);
	encounters.push_back(fourthEncounterUID);

	// Lasers
	lasers = GameplaySystems::GetGameObject(lasersUID);

	// Debug
	GameObject* shieldObj = GameplaySystems::GetGameObject(shieldObjUID);
	if (shieldObj) {
		dukeShield = GET_SCRIPT(shieldObj, DukeShield);
	}

	//Fire Tiles Script
	GameObject* tilesObj = GameplaySystems::GetGameObject(fireTilesUID);
	if (tilesObj) fireTilesScript = GET_SCRIPT(tilesObj, FloorIsLava);

	// AttackDronesController
	AttackDronesController* dronesController = GET_SCRIPT(&GetOwner(), AttackDronesController);

	// Dissolve Material
	GameObject* dissolveObj = GameplaySystems::GetGameObject(dissolveMaterialGOUID);
	if (dissolveObj) {
		ComponentMeshRenderer* dissolveMeshRenderer = dissolveObj->GetComponent<ComponentMeshRenderer>();
		if (dissolveMeshRenderer) {
			dissolveMaterialID = dissolveMeshRenderer->GetMaterial();
		}
	}

	// Init Duke character
	dukeCharacter.Init(dukeUID, playerUID, bulletUID, barrelUID, chargeColliderUID, meleeAttackColliderUID, barrelSpawnerUID, chargeAttackUID, phase2ShieldUID, encounters, dronesController, punchSlashUID, chargeDustUID, areaChargeUID, chargeTelegraphAreaUID);

	triggerBossEnd = GameplaySystems::GetGameObject(triggerBossEndUID);

	dukeCharacter.winSceneUID = winSceneUID; // TODO: REPLACE

	GameObject* hudManagerGO = GameplaySystems::GetGameObject(hudManagerUID);

	if (hudManagerGO) hudManager = GET_SCRIPT(hudManagerGO, HUDManager);
}

void AIDuke::Update() {
	if (!isReady) return;
	if (!player || !movementScript) return;
	if (GameplaySystems::GetGlobalVariable(globalIsGameplayBlocked, true)) return;

	/*
	std::string life = std::to_string(dukeCharacter.lifePoints);
	life = "Life points: " + life;
	Debug::Log(life.c_str());
	*/

	float speedToUse = dukeCharacter.slowedDown ? dukeCharacter.slowedDownSpeed : dukeCharacter.movementSpeed;

	if (dukeCharacter.mustAddAgent && dukeCharacter.agent) {
		dukeCharacter.agent->RemoveAgentFromCrowd();
		dukeCharacter.agent->AddAgentToCrowd();
		dukeCharacter.mustAddAgent = false;
	}

	if (dukeCharacter.slowedDown) {
		if (currentSlowedDownTime >= dukeCharacter.slowedDownTime) {
			dukeCharacter.agent->SetMaxSpeed(dukeCharacter.movementSpeed);
			dukeCharacter.slowedDown = false;
		}
		currentSlowedDownTime += Time::GetDeltaTime();
	}

	switch (phase) {
	case Phase::PHASE0:
		// Perform the "BOOM" animation
		if (dukeCharacter.compAnimation && mustPerformInitialAnimation) {
			dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::ENRAGE]); // TODO: change Enrage for the proper animation
			mustPerformInitialAnimation = false;
			dukeCharacter.state = DukeState::INVULNERABLE;
		} else if (dukeCharacter.state == DukeState::BASIC_BEHAVIOUR) phase = Phase::PHASE1;
		break;
	case Phase::PHASE1:
		currentShieldCooldown += Time::GetDeltaTime();
		if ((dukeCharacter.lifePoints < 0.85 * dukeCharacter.GetTotalLifePoints()) && !activeFireTiles) {
			Debug::Log("BulletHell active and fire tiles on");
			if (fireTilesScript) {
				fireTilesScript->StartFire();
				activeFireTiles = true;
			}
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
			if (dukeShield && dukeShield->GetIsActive()) {
				OnShieldInterrupted();
			}
			if (dukeCharacter.compAnimation) {
				dukeCharacter.StopShooting();
				dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::ENRAGE]);
			}
			dukeCharacter.state = DukeState::INVULNERABLE;
			if (fireTilesScript) {
				fireTilesScript->StopFire();
				fireTilesScript->SetInterphase(false);
				fireTilesScript->StartFire();
			}
			break;
		} else if (dukeCharacter.lifePoints < lifeThreshold * dukeCharacter.GetTotalLifePoints() && dukeCharacter.state != DukeState::BULLET_HELL && dukeCharacter.state != DukeState::CHARGE) {
			if(islevel2) { // only for level 2
				// "Fake" Duke death
				PerformDeath();
				SetReady(false);
				return;
			}
			phase = Phase::PHASE2;
			if (lasers && !lasers->IsActive()) lasers->Enable();
			Debug::Log("Lasers enabled");
			if (!phase2Reached) phase2Reached = true;
			// Phase change VFX?
			// Anim + dissolve for teleportation
			lifeThreshold -= 0.15f;

			Debug::Log("Fire tiles disabled");
			if (fireTilesScript && activeFireTiles) {
				fireTilesScript->StopFire();
				activeFireTiles = false;
			}
			movementScript->Stop();
			if (dukeCharacter.isInArena) dukeCharacter.TeleportDuke(true);

			// Second time Duke teleports out of the arena, there is a new fire pattern active.
			if (dukeCharacter.lifePoints <= 0.55f * dukeCharacter.GetTotalLifePoints()) {
				if (fireTilesScript) {
					fireTilesScript->SetInterphase(true);
					fireTilesScript->StartFire();
					activeFireTiles = true;
				}
			}
			dukeCharacter.state = DukeState::INVULNERABLE;
			if (dukeShield && dukeShield->GetIsActive()) {
				OnShieldInterrupted();
			}
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
			} else if (phase2Reached && playerController->playerOnimaru.shieldBeingUsed >= 2.0f && dukeCharacter.PlayerIsInChargeRangeDistance()) {
				// If onimaru shields -> perform charge
				movementScript->Stop();
				dukeCharacter.InitCharge(DukeState::BASIC_BEHAVIOUR);
			} else if (currentShieldCooldown >= shieldCooldown) {
				dukeCharacter.StartUsingShield();
			} else if (player && movementScript->CharacterInAttackRange(player, dukeCharacter.attackRange)) {
				// If player too close -> perform melee attack
				dukeCharacter.state = DukeState::MELEE_ATTACK;
				movementScript->Stop();
			} else {
				if (player) {
					if ((float3(13.0f, 0.799f, 0.0f) - player->GetComponent<ComponentTransform>()->GetGlobalPosition()).LengthSq() <
						(float3(13.0f, 0.799f, 0.0f) - ownerTransform->GetGlobalPosition()).LengthSq()) {
						// If player dominates the center for too long, perform charge
						timeSinceLastCharge += Time::GetDeltaTime();
					}
					if (timeSinceLastCharge >= 3.5f && dukeCharacter.PlayerIsInChargeRangeDistance()) {
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

			//Actual activating of the shield, when it is found not active during this state
			if (dukeShield && !dukeShield->GetIsActive()) {
				dukeShield->InitShield();
				movementScript->Stop();
			}

			dukeCharacter.Shoot();
			currentShieldActiveTime += Time::GetDeltaTime();
			if (currentShieldActiveTime >= shieldActiveTime) {

				OnShieldInterrupted();

				dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;

				dukeCharacter.StopShooting();
			}
			break;
		case DukeState::BULLET_HELL:
			PerformBulletHell();
			break;
		case DukeState::CHARGE:
			dukeCharacter.UpdateCharge();
			break;
		case DukeState::CHARGE_ATTACK:
			dukeCharacter.UpdateChargeAttack();
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
			UpdatePushStatus();
			break;
		default:
			break;
		}

		break;
	case Phase::PHASE2:
		if (dukeCharacter.isInArena) {
			Debug::Log("Fire tiles enabled");
			if (fireTilesScript) {
				if(activeFireTiles) fireTilesScript->StopFire();
				fireTilesScript->SetInterphase(false);
				fireTilesScript->StartFire();
				activeFireTiles = true;
			}
			phase = Phase::PHASE1;
			dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
			currentBulletHellCooldown = 0.f;
			currentShieldCooldown = 0.f;
		} else {
			if (dukeCharacter.phase2Shield && !dukeCharacter.phase2Shield->GetIsActive()) {
				dukeCharacter.StartPhase2Shield();
			}
			else {
				/*if (player) {
					float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition();
					movementScript->Orientate(dir);
				}*/
				currentBarrelTimer += Time::GetDeltaTime();
				if (currentBarrelTimer >= throwBarrelTimer) {
					dukeCharacter.ThrowBarrels();
					currentBarrelTimer = 0.f;
				}
			}
		}
		break;
	case Phase::PHASE3:
		if (dukeCharacter.lifePoints <= 0.f) {
			PerformDeath();
			return;

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
				if (currentAbilityChangeCooldown >= abilityChangeCooldown && dukeCharacter.PlayerIsInChargeRangeDistance()) {
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
			case DukeState::CHARGE_ATTACK:
				dukeCharacter.UpdateChargeAttack();
				break;
			case DukeState::MELEE_ATTACK:
				dukeCharacter.MeleeAttack();
				dukeCharacter.state = DukeState::BULLET_HELL;
				break;
			case DukeState::BULLET_HELL:
				PerformBulletHell();
				break;
			case DukeState::STUNNED:
				if (stunTimeRemaining <= 0.f) {
					stunTimeRemaining = 0.f;
					dukeCharacter.criticalMode = false;
					dukeCharacter.state = DukeState::SHOOT_SHIELD;
					movementScript->Stop();
					if (fireTilesScript) {
						fireTilesScript->StopFire();
						fireTilesScript->SetInterphase(true);
						fireTilesScript->StartFire();
					}
					dukeCharacter.CallTroops();
					dukeCharacter.StartUsingShield();
				} else {
					stunTimeRemaining -= Time::GetDeltaTime();
				}
				break;
			case DukeState::PUSHED:
				UpdatePushStatus();
				break;
			default:
				break;
			}
		} else {
			currentCriticalModeCooldown += Time::GetDeltaTime();
			if (currentCriticalModeCooldown >= criticalModeCooldown && dukeCharacter.state != DukeState::CHARGE && dukeCharacter.state != DukeState::BULLET_HELL) {
				currentCriticalModeCooldown = 0.f;
				dukeCharacter.criticalMode = true;
				movementScript->Stop();
				if (dukeShield && dukeShield->GetIsActive()) {
					OnShieldInterrupted();
				}
				if (dukeCharacter.compAnimation) {
					dukeCharacter.StopShooting();
					dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::ENRAGE]);
				}
				dukeCharacter.state = DukeState::INVULNERABLE;
				if (fireTilesScript) {
					fireTilesScript->StopFire();
					fireTilesScript->SetInterphase(false);
					fireTilesScript->StartFire();
				}
				return;
			}
			switch (dukeCharacter.state) {
			case DukeState::SHOOT_SHIELD:

				//Actual activating of the shield, when it is found not active during this state
				if (dukeShield && !dukeShield->GetIsActive()) {
					dukeShield->InitShield();
					movementScript->Stop();
				}

				movementScript->Orientate(player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition(), orientationSpeed, orientationThreshold);
				dukeCharacter.Shoot();
				currentAbilityChangeCooldown += Time::GetDeltaTime();
				if (currentAbilityChangeCooldown >= abilityChangeCooldown) {

					if (dukeShield && dukeShield->GetIsActive()) {
						OnShieldInterrupted();
					}
					currentAbilityChangeCooldown = 0.f;
					dukeCharacter.state = DukeState::BULLET_HELL;

					dukeCharacter.StopShooting();
				}
				break;
			case DukeState::BULLET_HELL:
				PerformBulletHell();
				break;
			case DukeState::BASIC_BEHAVIOUR:
				currentAbilityChangeCooldown += Time::GetDeltaTime();
				if (currentAbilityChangeCooldown >= abilityChangeCooldown) {
					currentAbilityChangeCooldown = 0.f;
						dukeCharacter.StartUsingShield();

				}
				else {
					if ((float3(13.0f, 0.799f, 0.0f) - player->GetComponent<ComponentTransform>()->GetGlobalPosition()).LengthSq() <
						(float3(13.0f, 0.799f, 0.0f) - ownerTransform->GetGlobalPosition()).LengthSq()) {
						// If player dominates the center for too long, perform charge
						timeSinceLastCharge += Time::GetDeltaTime();
					}
					if (timeSinceLastCharge >= 4.0f && dukeCharacter.PlayerIsInChargeRangeDistance()) {
						timeSinceLastCharge = 0.f;
						// Charge
						movementScript->Stop();
						dukeCharacter.InitCharge(DukeState::SHOOT_SHIELD);
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
				break;
			case DukeState::MELEE_ATTACK:
				dukeCharacter.MeleeAttack();
				break;
			case DukeState::CHARGE:
				dukeCharacter.UpdateCharge();
				break;
			case DukeState::CHARGE_ATTACK:
				dukeCharacter.UpdateChargeAttack();
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
				UpdatePushStatus();
				break;
			default:
				break;
			}
		}
		break;
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

		if (collidedWith.name == "FangBullet") {
			if (!particle) return;
			GameplaySystems::DestroyGameObject(&collidedWith);
			hitTaken = true;
			if (IsInvulnerable())return;
			float damage = playerController->playerFang.damageHit;
			if (CanBeFullyHurtDuringCriticalMode()) {
				dukeCharacter.GetHit(dukeCharacter.reducedDamaged ? damage / 2.f : damage + playerController->GetOverPowerMode());
			}
			else {
				// In critical mode only receives 1/3 damage
				dukeCharacter.GetHit(damage / 3.f + playerController->GetOverPowerMode());
			}
		}
		else if (collidedWith.name == "FangRightBullet" || collidedWith.name == "FangLeftBullet") {
			if (!particle) return;
			hitTaken = true;
			ParticleHit(collidedWith, particle, playerController->playerFang);
		}
		else if (collidedWith.name == "OnimaruBullet") {
			if (!particle) return;
			hitTaken = true;
			ParticleHit(collidedWith, particle, playerController->playerOnimaru);
		}
		else if (collidedWith.name == "OnimaruBulletUltimate") {
			if (!particle) return;
			hitTaken = true;
			ParticleHit(collidedWith, particle, playerController->playerOnimaru);
		}
		else if (collidedWith.name == "DashDamage" && playerController->playerFang.level1Upgrade) {
			hitTaken = true;
			if (IsInvulnerable()) return;
			float damage = playerController->playerFang.dashDamage;
			if (CanBeFullyHurtDuringCriticalMode()) {
				dukeCharacter.GetHit(dukeCharacter.reducedDamaged ? damage / 2.f : damage + playerController->GetOverPowerMode());
			}
			else {
				// In critical mode only receives 1/3 damage
				dukeCharacter.GetHit(damage / 3.f + playerController->GetOverPowerMode());
			}
    }

		if (hitTaken) {
			if (hudManager) hudManager->UpdateDukeHealth(dukeCharacter.lifePoints);
      // TODO: play audio and VFX
      /*if (audios[static_cast<int>(AudioType::HIT)]) audios[static_cast<int>(AudioType::HIT)]->Play();
      if (componentMeshRenderer) {
        if (damageMaterialID != 0) componentMeshRenderer->materialId = damageMaterialID;
      }

			timeSinceLastHurt = 0.0f;*/
		}


		if (collidedWith.name == "EMP" && dukeCharacter.state != DukeState::INVULNERABLE && dukeCharacter.state != DukeState::BULLET_HELL) {
			OnShieldInterrupted();
			dukeCharacter.BecomeStunned();
			dukeCharacter.StopShooting();
			movementScript->Stop();
			stunTimeRemaining = stunDuration;
			dukeCharacter.state = DukeState::STUNNED;
		}
	}

	if (!dukeCharacter.isAlive) {
		PerformDeath();
	}
}

void AIDuke::SetReady(bool value) {
	isReady = value;
}

void AIDuke::EnableBlastPushBack() {
	if (dukeCharacter.state != DukeState::INVULNERABLE && dukeCharacter.state != DukeState::BULLET_HELL) {
		dukeCharacter.beingPushed = true;
		dukeCharacter.state = DukeState::PUSHED;
		pushBackTimer = 0.f;
		dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::PUSHED]);
		dukeCharacter.StopShooting();
		dukeCharacter.CalculatePushBackFinalPos(ownerTransform->GetGlobalPosition(), player->GetComponent<ComponentTransform>()->GetGlobalPosition(), dukeCharacter.pushBackDistance);

		OnShieldInterrupted();

		// Damage
		if (playerController->playerOnimaru.level2Upgrade) {
			dukeCharacter.GetHit(playerController->playerOnimaru.blastDamage + playerController->GetOverPowerMode());
			if (hudManager) hudManager->UpdateDukeHealth(dukeCharacter.lifePoints);
			// TODO: play audio and VFX
			//if (audios[static_cast<int>(AudioType::HIT)]) audios[static_cast<int>(AudioType::HIT)]->Play();
			//PlayHitMaterialEffect();
			//timeSinceLastHurt = 0.0f;
		}

		dukeCharacter.BePushed();
		dukeCharacter.agent->Disable();
	}

}

void AIDuke::DisableBlastPushBack() {
	if (dukeCharacter.state != DukeState::INVULNERABLE) {
		dukeCharacter.beingPushed = false;
		//if (animation->GetCurrentState()) animation->SendTrigger(animation->GetCurrentState()->name + "Idle");
		dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
		dukeCharacter.agent->Enable();
		dukeCharacter.slowedDown = true;
		currentSlowedDownTime = 0.f;
	}
}

bool AIDuke::IsBeingPushed() const {
	return dukeCharacter.beingPushed;
}

void AIDuke::UpdatePushStatus() {
	if (pushBackTimer > dukeCharacter.pushBackTime) {
		pushBackTimer = dukeCharacter.pushBackTime;
	}

	UpdatePushBackPosition();

	if (pushBackTimer == dukeCharacter.pushBackTime) {
		DisableBlastPushBack();
	}
	else {
		pushBackTimer += Time::GetDeltaTime();
	}
}

void AIDuke::UpdatePushBackPosition() {
	ownerTransform->SetGlobalPosition(float3::Lerp(dukeCharacter.pushBackInitialPos, dukeCharacter.pushBackFinalPos, pushBackTimer / dukeCharacter.pushBackTime));
}

void AIDuke::ParticleHit(GameObject& collidedWith, void* particle, Player& player_) {
	if (!particle) return;
	ComponentParticleSystem::Particle* p = (ComponentParticleSystem::Particle*)particle;
	ComponentParticleSystem* pSystem = collidedWith.GetComponent<ComponentParticleSystem>();
	if (pSystem) pSystem->KillParticle(p);
	float damage = dukeCharacter.reducedDamaged ? player_.damageHit / 2.f : player_.damageHit;

	if (IsInvulnerable())return;

	if (!dukeCharacter.criticalMode) {
		if (dukeCharacter.state == DukeState::STUNNED && player_.level2Upgrade) {
			dukeCharacter.GetHit(damage * 2.f + playerController->GetOverPowerMode());
		} else {
			dukeCharacter.GetHit(damage + playerController->GetOverPowerMode());
		}
	} else {
		if (CanBeFullyHurtDuringCriticalMode()) {
			if (dukeCharacter.state == DukeState::STUNNED && player_.level2Upgrade) {
				dukeCharacter.GetHit(damage * 2.f + playerController->GetOverPowerMode());
			}
			else {
				dukeCharacter.GetHit(damage + playerController->GetOverPowerMode());
			}
		}
		else {
			dukeCharacter.GetHit(player_.damageHit / 3.f + playerController->GetOverPowerMode());
		}
	}

}

bool AIDuke::CanBeFullyHurtDuringCriticalMode() const {
	return dukeCharacter.slowedDown || IsBeingPushed() || dukeCharacter.state == DukeState::STUNNED;
}

bool AIDuke::IsInvulnerable() const {
	return dukeCharacter.state == DukeState::INVULNERABLE;
}




//To be called to reset everything and Fade shield whenever was shielding and got pushed/stunned or straight up stopped shielding
void AIDuke::OnShieldInterrupted() {
	if (dukeShield&&dukeShield->GetIsActive()) {
		dukeShield->FadeShield();
	}
	currentShieldCooldown = 0.f;
	currentShieldActiveTime = 0.f;
}

void AIDuke::PerformBulletHell() {
	if (mustWaitForTimerBetweenAbilities) {
		if (currentTimeBetweenAbilities >= timerBetweenAbilities) {
			mustWaitForTimerBetweenAbilities = false;
			currentTimeBetweenAbilities = 0.0f;
		}
		else {
			currentTimeBetweenAbilities += Time::GetDeltaTime();

			float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition();
			dir.y = 0.0f;
			movementScript->Orientate(dir);
			dukeCharacter.Move(dir);

			return;
		}
	}
	movementScript->Stop();
	dukeCharacter.reducedDamaged = true;

	if (!bulletHellIsActive) {
		dukeCharacter.BulletHell();
		bulletHellIsActive = true;
	}

	if (!dukeCharacter.IsBulletHellCircular()) {
		float3 dir = player->GetComponent<ComponentTransform>()->GetGlobalPosition() - ownerTransform->GetGlobalPosition();
		dir.y = 0.0f;
		movementScript->Orientate(dir, orientationSpeedBulletHell, orientationThresholdBulletHell);
	}

	if (dukeCharacter.BulletHellFinished()) {
		dukeCharacter.DisableBulletHell();
		bulletHellIsActive = false;
		dukeCharacter.reducedDamaged = false;
		currentBulletHellCooldown = 0.f;
		dukeCharacter.state = DukeState::BASIC_BEHAVIOUR;
		currentTimeBetweenAbilities = 0.0f;
		mustWaitForTimerBetweenAbilities = true;
	}
}

void AIDuke::PerformDeath() {
	movementScript->Stop();
	OnShieldInterrupted();
	dukeCharacter.StopShooting();
	dukeCharacter.compAnimation->SendTrigger(dukeCharacter.compAnimation->GetCurrentState()->name + dukeCharacter.animationStates[Duke::DUKE_ANIMATION_STATES::DEATH]);

	// TODO: play audio and VFX
	//if (audios[static_cast<int>(AudioType::DEATH)]) audios[static_cast<int>(AudioType::DEATH)]->Play();
	ComponentCapsuleCollider* collider = GetOwner().GetComponent<ComponentCapsuleCollider>();
	if (collider) collider->Disable();

	dukeCharacter.agent->RemoveAgentFromCrowd();
	if (playerController) playerController->RemoveEnemyFromMap(duke);
	if (dukeCharacter.beingPushed) dukeCharacter.beingPushed = false;
	dukeCharacter.state = DukeState::DEATH;

	// Activate the combat end trigger. This will activate a dialogue and dissolve Duke in level 2.
	if (triggerBossEnd) triggerBossEnd->Enable();
	
	// Stop environment hazards
	if (!islevel2) {
		if (activeFireTiles && fireTilesScript) fireTilesScript->StopFire();
		if (lasers && lasers->IsActive()) lasers->Disable();
		// TODO: Substitute the following for actual destruction of the troops
		GameObject* encounter = GameplaySystems::GetGameObject(fourthEncounterUID);
		if (encounter && encounter->IsActive()) encounter->Disable();
	}
}

float AIDuke::GetDukeMaxHealth() const {
	return dukeCharacter.GetTotalLifePoints();
}

void AIDuke::ActivateDissolve() {
	dukeCharacter.ActivateDissolve(dissolveMaterialID);
}
