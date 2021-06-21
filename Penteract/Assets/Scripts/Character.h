#pragma once

class ComponentAudioSource;
class ComponentAgent;
class ComponentAnimation;
class ComponentTransform;

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
	ComponentAgent* agent = nullptr;
	ComponentAnimation* compAnimation = nullptr;
	ComponentTransform* characterTransform = nullptr;

private:
	int totalLifePoints = 1;
	
};
