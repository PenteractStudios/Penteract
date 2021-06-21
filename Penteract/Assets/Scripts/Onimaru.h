#pragma once

#include "Player.h"

enum class AudioOnimaru {
	SHIELD,
	BLAST,
	ULTIMATE,
	SWITCH,
	SHOOT,
	HIT,
	DEATH,
	TOTAL
};

class Onimaru : public Player {
public:
	// ------- Contructors ------- //
	Onimaru() {};

	Onimaru(int lifePoints_, float movementSpeed_, int damageHit_, float attackSpeed_)
	{
		attackSpeed = attackSpeed_;
		lifePoints = lifePoints_;
		movementSpeed = movementSpeed_;
		damageHit = damageHit_;
		SetTotalLifePoints(lifePoints);
	}

public:
	
	ComponentAudioSource* audios[static_cast<int>(AudioOnimaru::TOTAL)] = { nullptr };

};
