#pragma once

constexpr const char* globalVariableKeyPlayVideoScene1 = "CameFromMainMenu";

//constexpr const char* globalIsGameplayBlocked = "IsGameplayBlocked";	// IsGameplayBlocked is used to stop the gameplay without pausing the game itself. When true, all entities will remain in an IDLE state, and player input will be mostly ignored.

// Tutorials - these booleans control when the skills are available to the player
constexpr const char* globalSkill1TutorialReached = "Skill1Reached";	// Dash/Shield
constexpr const char* globalSkill2TutorialReached = "Skill2Reached";	// EMP/Blast
constexpr const char* globalSkill3TutorialReached = "Skill3Reached";	// Ultimate
constexpr const char* globalSwitchTutorialReached = "SwitchReached";	// Switch
