#include "RobotsAssemblyMovement.h"
#include "Components/ComponentTransform.h"
#include "Resources/ResourcePrefab.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(RobotsAssemblyMovement) {
	MEMBER_SEPARATOR("Robot line variables"),
	MEMBER(MemberType::FLOAT, distanceBetweenRobots),
	MEMBER(MemberType::INT, numOfRobots),
	MEMBER(MemberType::FLOAT, speed),
	MEMBER(MemberType::BOOL, changeDirection),
	MEMBER(MemberType::FLOAT, lineLenght),
	MEMBER_SEPARATOR("Stop variables"),
	MEMBER(MemberType::FLOAT, distanceBetweenStops),
	MEMBER(MemberType::FLOAT, stopTime),
	MEMBER_SEPARATOR("Robots prefabs"),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, robotType1),
};

GENERATE_BODY_IMPL(RobotsAssemblyMovement);

void RobotsAssemblyMovement::Start() {
	robotsLine = &GetOwner();
	std::vector<GameObject*> children = robotsLine->GetChildren();
	if (children.size() == 1) {
		ComponentTransform* transform = children[0]->GetComponent<ComponentTransform>();
		if (transform) {
			initialPos = transform->GetGlobalPosition();
			direction = transform->GetFront();
		}
	}
	ResourcePrefab* robotPrefab = GameplaySystems::GetResource<ResourcePrefab>(robotType1);
	GameObject* robot = nullptr;
	forward = changeDirection ? -1 : 1;
	if (robotPrefab) {
		for (int i = 1; i < numOfRobots; ++i) {
			robot = GameplaySystems::GetGameObject(robotPrefab->BuildPrefab(robotsLine));

			if (robot) {
				ComponentTransform* transform = robot->GetComponent<ComponentTransform>();
				if (transform) {
					float3 newPos = initialPos + direction * (i * distanceBetweenRobots) * forward;
					transform->SetGlobalPosition(newPos);
				}
			}
		}
	}

	robots = robotsLine->GetChildren();
	finalPos = initialPos + direction * lineLenght;
}

void RobotsAssemblyMovement::Update() {
	if (robots.size() != numOfRobots) return;

	ComponentTransform* robotTransform = nullptr;

	if (!robotsStopped) {
		for (GameObject* robot : robots) {
			robotTransform = robot->GetComponent<ComponentTransform>();
			float3 position = robotTransform->GetGlobalPosition();

			float3 newPosition = initialPos;

			if (initialPos.Distance(position) <= lineLenght) {
				newPosition = position + direction * speed * Time::GetDeltaTime() * forward;
			}
			robotTransform->SetGlobalPosition(newPosition);
		}
		currentDistanceBetweenStops += speed * Time::GetDeltaTime();

		if (currentDistanceBetweenStops > distanceBetweenStops) {
			robotsStopped = true;
			currentDistanceBetweenStops = 0.f;
		}
	}
	else {
		if (stopTimer >= stopTime) {
			robotsStopped = false;
			stopTimer = 0.f;
		}
		else stopTimer += Time::GetDeltaTime();
	}
}