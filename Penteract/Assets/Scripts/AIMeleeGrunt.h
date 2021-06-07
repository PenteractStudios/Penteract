#pragma once

#include "Scripting/Script.h"

#include "AIState.h"
#include "Enemy.h"

class GameObject;
class ComponentAnimation;
class ComponentTransform;
class ComponentAgent;
class ComponentAudioSource;

class HUDController;
class PlayerController;
class AIMovement;

class AIMeleeGrunt : public Script
{
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
	void HitDetected(int damage_ = 1);

public:

	UID playerUID = 0;
	UID canvasUID = 0;

	GameObject* player = nullptr;
	ComponentAgent* agent = nullptr;

	Enemy gruntCharacter = Enemy(5, 8.0f, 1, 30, 40.f, 5.f, 5.f);

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

	ComponentAudioSource* audios[static_cast<int>(AudioType::TOTAL)];

};

