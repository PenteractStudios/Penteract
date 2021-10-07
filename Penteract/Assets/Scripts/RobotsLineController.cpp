#include "RobotsLineController.h"

#include "Components/ComponentTransform.h"
#include "Resources/ResourcePrefab.h"

#include "RobotLineMovement.h"

#include "GameplaySystems.h"

EXPOSE_MEMBERS(RobotsLineController) {
	MEMBER_SEPARATOR("Robot line variables"),
	MEMBER(MemberType::FLOAT, timeBetweenSpawns),
	MEMBER(MemberType::FLOAT, timeBetweenStops),
	MEMBER(MemberType::FLOAT, timeToReachDestination),
	MEMBER(MemberType::FLOAT, timeStopped),
	MEMBER(MemberType::BOOL, changeDirection),
	MEMBER(MemberType::FLOAT, lineLength),
	MEMBER_SEPARATOR("Robots prefabs"),
	MEMBER(MemberType::PREFAB_RESOURCE_UID, prefab)
};

GENERATE_BODY_IMPL(RobotsLineController);

void RobotsLineController::Start() {
	robotsParent = GetOwner().GetChild("Robots");
	std::vector<GameObject*> children = robotsParent->GetChildren();
	// Get the first robot reference
	if (children.size() > 0) {
		ComponentTransform* transform = children[0]->GetComponent<ComponentTransform>();
		if (transform) {
			initialPos = transform->GetGlobalPosition();
			direction = transform->GetFront();
		}
		forwardReversed = changeDirection ? -1 : 1;
		finalPos = initialPos + direction * lineLength * forwardReversed;
		RobotLineMovement* script = GET_SCRIPT(children[0], RobotLineMovement);
		if (script) script->Initialize(initialPos, finalPos, timeToReachDestination);
	}
	robotPrefab = GameplaySystems::GetResource<ResourcePrefab>(prefab);
	spawnNextRobotTimer = timeBetweenSpawns;
	stoppedTimer = timeStopped;
	stopInTimer = timeBetweenStops;

}

void RobotsLineController::Update() {
	if (!robotPrefab) return;
	if (!robotsStopped) {
		if (spawnNextRobotTimer <= 0.f) {
			GameObject* robot = GameplaySystems::GetGameObject(robotPrefab->BuildPrefab(robotsParent));
			spawnNextRobotTimer = timeBetweenSpawns;
			RobotLineMovement* script = GET_SCRIPT(robot, RobotLineMovement);
			if (script) script->Initialize(initialPos, finalPos, timeToReachDestination);
		}
		else {
			spawnNextRobotTimer -= Time::GetDeltaTime();
		}

		if (stopInTimer <= 0.f) {
			for (GameObject* robot : robotsParent->GetChildren()) {
				RobotLineMovement* script = GET_SCRIPT(robot, RobotLineMovement);
				script->Stop();
				stoppedTimer = timeStopped;
			}
			robotsStopped = true;
		}
		else {
			stopInTimer -= Time::GetDeltaTime();
		}
	}
	else {
		if (stoppedTimer <= 0.f) {
			for (GameObject* robot : robotsParent->GetChildren()) {
				RobotLineMovement* script = GET_SCRIPT(robot, RobotLineMovement);
				script->Restart();
				stopInTimer = timeBetweenStops;
			}
			robotsStopped = false;
		}
		else {
			stoppedTimer -= Time::GetDeltaTime();
		}
	}

	GameObject* robotToDestroy = robotsParent->GetChildren()[0];

	if (robotToDestroy) {
		RobotLineMovement* script = GET_SCRIPT(robotToDestroy, RobotLineMovement);
		bool destroy = script->NeedsToBeDestroyed();
		if (destroy) GameplaySystems::DestroyGameObject(robotToDestroy);
	}
}