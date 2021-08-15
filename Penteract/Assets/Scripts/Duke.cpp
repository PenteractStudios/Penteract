#include "Duke.h"

void Duke::Init(UID dukeUID, UID playerUID)
{
}

void Duke::ShootAndMove(const float3& newPosition)
{
	Debug::Log("I'm moving while shooting");
}

void Duke::MeleeAttack()
{
	Debug::Log("Hooryah!");
}

void Duke::ShieldShoot()
{
	Debug::Log("I'm shielding while shooting at your face");
}

void Duke::BulletHell()
{
	Debug::Log("Bullet hell");
}

void Duke::Charge()
{
	Debug::Log("Electric Tackle!");
}

void Duke::CallTroops()
{
	Debug::Log("Come, guys!");
}
