#pragma once

#include "Components/Component.h"
#include "Components/ComponentParticleSystem.h"
#include "LinearMath/btMotionState.h"
#include "LinearMath/btTransform.h"

#include "Math/float3.h"

class ParticleMotionState : public btMotionState {
public:
	ParticleMotionState(ComponentParticleSystem::Particle* p);
	~ParticleMotionState();

	void getWorldTransform(btTransform& centerOfMassWorldTrans) const override;
	void setWorldTransform(const btTransform& centerOfMassWorldTrans) override;

private:
	ComponentParticleSystem::Particle* particle = nullptr;
};