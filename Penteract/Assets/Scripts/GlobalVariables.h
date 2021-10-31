#pragma once

constexpr const char* globalCheckpoint = "Checkpoint";			// This variable keeps track of the last reached checkpoint. When the player dies, he will be able to resume the game at that point.

constexpr const char* globalVariableKeyPlayVideoScene1 = "CameFromMainMenu";
constexpr const char* isVideoActive = "IsVideoActive";			// isVideoActive is used to check if any video is Active and playing on the scene.

constexpr const char* globalIsGameplayBlocked = "IsGameplayBlocked";	// IsGameplayBlocked is used to stop the gameplay without pausing the game itself. When true, all entities will remain in an IDLE state, and player input will be mostly ignored.
constexpr const char* globalCameraEventOn = "CameraEventOn"; //Is some camera event happening pausing the game itself.

constexpr const char* globalMovePlayerFromCode = "MovePlayerFromCode"; // MovePlayerFromCode is used to allow to move the Player from code while the gameplay is blocked, used during the "gameplay scenes" in Boss level with Player::MoveTo(newPosition).

// Tutorials - these booleans control when the skills are available to the player
constexpr const char* globalSkill1TutorialReached = "Skill1Reached";		// Dash
constexpr const char* globalSkill2TutorialReached = "Skill2Reached";		// EMP
constexpr const char* globalSkill3TutorialReached = "Skill3Reached";		// Fang Ultimate
constexpr const char* globalSwitchTutorialReached = "SwitchReached";		// Switch
constexpr const char* globalSkill1TutorialReachedOni = "Skill1ReachedOni";	// Shield
constexpr const char* globalSkill2TutorialReachedOni = "Skill2ReachedOni";	// Blast
constexpr const char* globalSkill3TutorialReachedOni = "Skill3ReachedOni";	// Oni Ultimate
constexpr const char* globalswitchTutorialActive = "IsSwitchTutorialActive";	// This overrides globalSwitchTutorialReached on Fang's 'CanSwitch()', used when the Switch Tutorial appears to allow the use of the Switch then.

constexpr const char* globalBackDetectionInUse = "BackDetectionInUse"; //globalBackDetectionInUse determines if cancel buttons must do navigation stuff or just UnPause, a UINavigationBackDetection script with nullreferences should only be found on the resume button, thus only with that button calling ONEnable should this be set to false
constexpr const char* globalUseGamepad = "UseGamepad";
