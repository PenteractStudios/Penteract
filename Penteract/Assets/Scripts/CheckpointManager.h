#pragma once

#include "Scripting/Script.h"
#include "UIAudioType.h"

#define N_CHECKPOINTS 5

class ComponentAudioSource;
class ComponentAgent;
class ComponentSelectable;
class GameObject;
class PlayerController;

/// <summary>
/// This class should only have an instance on a particular scene, it will check the player's position every X seconds
/// If the player gets distanceThreshold close to one of the checkpoint positions, that one is set as checkpoint
/// Resetting the level through the canvas button that calls ReloadScene will reset player pos to last checkpoint
/// </summary>
class CheckpointManager : public Script {
	GENERATE_BODY(CheckpointManager);

public:

	void Start() override;
	void Update() override;
	void OnButtonClick() override;

	void PlayAudio(UIAudio type);
public:
	UID avatarUID;
	GameObject* avatarObj;
	PlayerController* playerScript = nullptr;
	float3 checkpointPosition1, checkpointPosition2, checkpointPosition3, checkpointPosition4, checkpointPosition5;
	float distanceThreshold, timeBetweenChecks, timeBetweenChecksCounter;

	/* Triggers Object */
	UID triggersUID;
	GameObject* triggers = nullptr;
	std::vector<GameObject*> listTriggers;

	/* Encounters Level */
	UID encounter1UID, encounter2UID, encounter3UID, encounter4UID, encounter5UID, encounter6UID, encounter7UID;
	GameObject* encounter1 = nullptr;
	GameObject* encounter2 = nullptr;
	GameObject* encounter3 = nullptr;
	GameObject* encounter4 = nullptr;
	GameObject* encounter5 = nullptr;
	GameObject* encounter6 = nullptr;
	GameObject* encounter7 = nullptr;

	/* Doors Level */
	UID doors1UID, doors2UID, doors3UID, doors4UID, doors5UID;
	GameObject* doors1 = nullptr;
	GameObject* doors2 = nullptr;
	GameObject* doors3 = nullptr;
	GameObject* doors4 = nullptr;
	GameObject* doors5 = nullptr;

	/* Dialogs Level */
	UID dialogs1UID, dialogs2UID, dialogs3UID, dialogs4UID, dialogs5UID;
	GameObject* dialogs1 = nullptr;
	GameObject* dialogs2 = nullptr;
	GameObject* dialogs3 = nullptr;
	GameObject* dialogs4 = nullptr;
	GameObject* dialogs5 = nullptr;

	/* Videos Level */
	UID video1UID, video2UID;
	GameObject* video1 = nullptr;
	GameObject* video2 = nullptr;

private:
	void CheckDistanceWithCheckpoints();

private:
	ComponentAgent* agent = nullptr;
	float3 runtimeCheckpointPositions[N_CHECKPOINTS];
	bool playHoveredAudio = true;
	ComponentSelectable* selectable = nullptr;
	ComponentAudioSource* audios[static_cast<int>(UIAudio::TOTAL)] = { nullptr };
	bool dirty = true;
};

