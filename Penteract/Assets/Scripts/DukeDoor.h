#include "Scripting/Script.h"

class GameObject;
class DialogueManager;
class AIDuke;
class PlayerController;
class CameraController;
class MovingLasers;

class DukeDoor : public Script
{
	GENERATE_BODY(DukeDoor);
public:
	void Start() override;
	void Update() override;
	void OnCollision(GameObject& collidedWith, float3 collisionNormal, float3 penetrationDistance, void* particle = nullptr) override;

private:
	void PlayDissolveAnimation(GameObject* root, bool playReverse);

public:
	UID playerUID = 0;
	UID dukeUID = 0;
	UID doorObstacleUID = 0;
	UID canvasHUDUID = 0;
	UID gameControllerUID = 0;
	UID gameCameraUID = 0;
	int dialogueID = 0;
	float3 initialTalkPosition = float3(7.f, 1.f, 0.f);

	UID optionalExitDoorObstacleUID = 0;
	UID optionalLaserUID = 0;


	/* Dissolve UID */
	UID dissolveMaterialGOUID = 0;

private:
	GameObject* player = nullptr;
	GameObject* obstacle = nullptr;
	DialogueManager* dialogueManagerScript = nullptr;
	AIDuke* aiDuke = nullptr;
	PlayerController* playerController = nullptr;
	CameraController* camera = nullptr;

	GameObject* exitObstacle = nullptr;
	MovingLasers* optionalLaserScript = nullptr;

	/* Dissolve UID */
	UID dissolveMaterialID = 0;


	/* Scene flow triggers */
	bool triggered = false;			// Two objects can collide with the trigger at the same time. This boolean prevents the OnCollision() code to be executed twice when that happens
	bool startDialogue = false;		// This triggers the dialogue between Fang and Duke, and it is set to true when Fang arrives in front of Duke.
	bool finishScene = false;		// This will start the combat after duke performs the RAGE animation
};
