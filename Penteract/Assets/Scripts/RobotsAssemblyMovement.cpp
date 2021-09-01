#include "RobotsAssemblyMovement.h"
#include "Components/ComponentTransform.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(RobotsAssemblyMovement) {
	MEMBER(MemberType::FLOAT, offset),
	MEMBER(MemberType::BOOL, onwards),
	MEMBER(MemberType::FLOAT, speed)
};

GENERATE_BODY_IMPL(RobotsAssemblyMovement);

void RobotsAssemblyMovement::Start() {
	robots = &GetOwner();
	robotsTransform = robots->GetComponent<ComponentTransform>();

	if (robotsTransform) initialPos = robotsTransform->GetGlobalPosition();

	direction = float3(initialPos.x, initialPos.y, initialPos.z - offset) - initialPos;
}

void RobotsAssemblyMovement::Update() {
	if (distanceMoved < offset) {
		float3 position = robotsTransform->GetGlobalPosition();

		float3 newPosition = position + direction * speed * Time::GetDeltaTime();

		distanceMoved += newPosition.Distance(position);

		robotsTransform->SetGlobalPosition(newPosition);
	}
	else {
		distanceMoved = 0.f;
		robotsTransform->SetGlobalPosition(initialPos);
	}
}