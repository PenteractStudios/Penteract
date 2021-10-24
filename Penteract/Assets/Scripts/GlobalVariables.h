#pragma once

constexpr const char* globalCheckpoint = "Checkpoint";			// This variable keeps track of the last reached checkpoint. When the player dies, he will be able to resume the game at that point.
constexpr const char* globalLevel = "Level";					// This variable keeps track of the last reached level. When the player dies, he will be able to resume the game at that level.

constexpr const char* globalVariableKeyPlayVideoScene1 = "CameFromMainMenu";
constexpr const char* isVideoActive = "IsVideoActive";			// isVideoActive is used to check if any video is Active and playing on the scene.

constexpr const char* globalIsGameplayBlocked = "IsGameplayBlocked";	// IsGameplayBlocked is used to stop the gameplay without pausing the game itself. When true, all entities will remain in an IDLE state, and player input will be mostly ignored.

// Tutorials - these booleans control when the skills are available to the player
constexpr const char* globalSkill1TutorialReached = "Skill1Reached";	// Dash/Shield
constexpr const char* globalSkill2TutorialReached = "Skill2Reached";	// EMP/Blast
constexpr const char* globalSkill3TutorialReached = "Skill3Reached";	// Ultimate
constexpr const char* globalSwitchTutorialReached = "SwitchReached";	// Switch
constexpr const char* globalswitchTutorialActive = "IsSwitchTutorialActive";	// This overrides globalSwitchTutorialReached on Fang's 'CanSwitch()', used when the Switch Tutorial appears to allow the use of the Switch then.

