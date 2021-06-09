#pragma once

#include "Scripting/Script.h"

#include "AIState.h"
#include "Enemy.h"

class GameObject;
class ComponentAnimation;
class ComponentTransform;
class ComponentAgent;
class ComponentAudioSource;
class ResourcePrefab;
class HUDController;
class PlayerController;
class AIMovement;
class WinLose;
class EnemySpawnPoint;

class AIMeleeGrunt : public Script {
	GENERATE_BODY(AIMeleeGrunt);

public:
	enum class AudioType {
		SPAWN,
		ATTACK,
		HIT,
		DEATH,
		TOTAL
	};

	void Start() override;
	void Update() override;
	void OnAnimationFinished() override;
	void OnAnimationSecondaryFinished() override;
	void OnCollision(GameObject& collidedWith) override;

public:

	UID playerUID = 0;
	UID canvasUID = 0;
	UID winConditionUID = 0;
	UID meleePunchUID = 0;

	GameObject* player = nullptr;
	GameObject* spawn = nullptr;
	ComponentAgent* agent = nullptr;
	ResourcePrefab* meleePunch = nullptr;
	WinLose* winLoseScript = nullptr;

	Enemy gruntCharacter = Enemy(5, 8.0f, 1, 30, 40.f, 5.f, 5.f);
	bool killSent = false;

private:

	float3 velocity = float3(0, 0, 0);
	AIState state = AIState::START;
	bool hitTaken = false;
	ComponentAnimation* animation = nullptr;
	ComponentTransform* ownerTransform = nullptr;
	int damageRecieved = 0;

	HUDController* hudControllerScript = nullptr;
	PlayerController* playerController = nullptr;
	AIMovement* movementScript = nullptr;
	EnemySpawnPoint* enemySpawnPointScript = nullptr;

	ComponentAudioSource* audios[static_cast<int>(AudioType::TOTAL)] = { nullptr };

};
