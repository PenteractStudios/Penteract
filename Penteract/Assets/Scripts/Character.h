#pragma once

class ComponentAudioSource;
class ComponentAgent;
class ComponentAnimation;
class ComponentTransform;

enum class AudioType {
	DASH,
	SWITCH,
	SHOOT,
	FANGHIT,
	ONIHIT,
	FANGDEATH,
	ONIDEATH,
	TOTAL
};
class Character
{
public:
	Character() {}

	void Hit(int damage_);
	void Recover(int recoveryLife_);

	void SetTotalLifePoints(int totalLifePoints_);
	void SetDamageHit(int damageHit_);

public:
	bool isAlive = true;
	int lifePoints = 1;
	int damageHit = 1;
	float movementSpeed = 1.0f;
	ComponentAudioSource* audios[static_cast<int>(AudioType::TOTAL)] = { nullptr };
	ComponentAgent* agent = nullptr;
	ComponentAnimation* compAnimation = nullptr;
	ComponentTransform* characterTransform = nullptr;

private:
	int totalLifePoints = 1;
	
};
