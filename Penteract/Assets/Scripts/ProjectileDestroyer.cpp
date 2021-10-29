#include "ProjectileDestroyer.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(ProjectileDestroyer) {
};

GENERATE_BODY_IMPL(ProjectileDestroyer);

void ProjectileDestroyer::Start() {
	
}

void ProjectileDestroyer::Update() {
}

void ProjectileDestroyer::OnCollision(GameObject& collidedWith, float3 /*collisionNormal*/, float3 /*penetrationDistance*/, void* particle) {
	if (collidedWith.name == "DukeProjectile") {
		if (!particle) return;
		ComponentParticleSystem::Particle* p = (ComponentParticleSystem::Particle*)particle;
		ComponentParticleSystem* pSystem = collidedWith.GetComponent<ComponentParticleSystem>();
		if (pSystem && p) pSystem->KillParticle(p);
	}
}