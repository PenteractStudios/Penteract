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
	std::vector<GameObject*> children;
	if (robotsParent) {
		children = robotsParent->GetChildren();
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
	}
	robotPrefab = GameplaySystems::GetResource<ResourcePrefab>(prefab);
	spawnNextRobotTimer = timeBetweenSpawns;
	stoppedTimer = timeStopped;
	stopInTimer = timeBetweenStops;

	totalRobotsToDeploy = timeToReachDestination / timeBetweenSpawns;

}

void RobotsLineController::Update() {
	if (!robotPrefab || !robotsParent) return;
	if (!robotsStopped) {
		if (spawnNextRobotTimer <= 0.f) {
			if (!allRobotsDeployed) {
				GameObject* robot = GameplaySystems::GetGameObject(robotPrefab->BuildPrefab(robotsParent));
				if (robot) {
					RobotLineMovement* script = GET_SCRIPT(robot, RobotLineMovement);
					if (script) script->Initialize(initialPos, finalPos, timeToReachDestination);
				}
				++robotsDeployed;
				allRobotsDeployed = robotsDeployed == totalRobotsToDeploy;
				spawnNextRobotTimer = timeBetweenSpawns;
			}
			else {
				if (robotsParent->GetChildren().size() > robotToMove) {
					GameObject* robotToResetPos = robotsParent->GetChildren()[robotToMove];
					RobotLineMovement* script = GET_SCRIPT(robotToResetPos, RobotLineMovement);
					if (script) {
						bool move = script->NeedsToBeReset();
						if (move) {
							script->ResetMovement();
							robotToMove += 1;
							robotToMove = robotToMove == totalRobotsToDeploy ? 0 : robotToMove;
							spawnNextRobotTimer = timeBetweenSpawns;
						}
					}
				}
			}
		}
		else {
			spawnNextRobotTimer -= Time::GetDeltaTime();
		}

		if (stopInTimer <= 0.f) {
			for (GameObject* robot : robotsParent->GetChildren()) {
				RobotLineMovement* script = GET_SCRIPT(robot, RobotLineMovement);
				if (script) script->Stop();
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
				if (script) script->Restart();
				stopInTimer = timeBetweenStops;
			}
			robotsStopped = false;
		}
		else {
			stoppedTimer -= Time::GetDeltaTime();
		}
	}
}