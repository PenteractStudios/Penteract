#pragma once

#include "Module.h"

#include "Math/float2.h"
#include "SDL_scancode.h"
#include "SDL_gamecontroller.h"

#define NUM_MOUSE_BUTTONS 5
#define MAX_PLAYERS 2

enum KeyState {
	KS_IDLE = 0,
	KS_DOWN,
	KS_REPEAT,
	KS_UP
};

/// <summary>
/// Class that contains two arrays, one for Game controller buttons and one for Game controller axises, a joystick index (used mostly for SDL internal purposes) and a reference to de SDL_Gamecontroller* for proper deletion
/// </summary>
class PlayerController {
public:
	PlayerController(int index_) {
		controller = SDL_GameControllerOpen(index_);
		SDL_Joystick* j = SDL_GameControllerGetJoystick(controller);
		joystickIndex = SDL_JoystickInstanceID(j);
	}

	~PlayerController() {
		if (controller != NULL) {
			SDL_GameControllerClose(controller);
			controller = NULL;
		}
	}

	float GetAxis(int axisIndex) {
		return gameControllerAxises[axisIndex];
	}

	KeyState GetButtonState(int buttonIndex) {
		return gameControllerButtons[buttonIndex];
	}

public:
	//PlayerGamepad id is NOT the array position nor the device index, due to how SDL manages controllers it is the controller's joystick's index, use carefully
	int joystickIndex = -1;
	float gameControllerAxises[SDL_CONTROLLER_AXIS_MAX] = {0.0f};		   // Axis values, deadzone is 8000, max value is
	KeyState gameControllerButtons[SDL_CONTROLLER_BUTTON_MAX] = {KS_IDLE}; // Same keystate, but for the controller buttons
	SDL_GameController* controller = nullptr;
};

class ModuleInput : public Module {
public:
	const float JOYSTICK_DEAD_ZONE = 8000.0f;  //Hardcoded value considered Game controller "Dead zone"
	const float JOYSTICK_MAX_VALUE = 32767.0f; //MAXIUM RAW DATA obtainable through axis

public:
	// ------- Core Functions ------ //
	bool Init() override;
	UpdateStatus PreUpdate() override; // All button and keys pressed are processed here.
	bool CleanUp() override;

	void ReleaseDroppedFilePath();			// Calls a SDL_free() to release 'droppedFilePath'.
	void WarpMouse(int mouseX, int mouseY); // Places the mouse in a given X,Y position in the screen. Called from ModuleCamera when rotating or orbiting, to allow rotating infinitely.

	// ---------- Getters ---------- //
	const char* GetDroppedFilePath() const;
	KeyState GetKey(int scancode) const;
	KeyState GetMouseButton(int button) const;
	float GetMouseWheelMotion() const;
	const float2& GetMouseMotion() const;
	const float2& GetMousePosition(bool sceneSensitive = false) const;
	KeyState* GetMouseButtons();
	KeyState* GetKeyboard();

	void OnControllerAdded(int index);	 //Callback for whenever a new game controller is plugged in, currently handling up to two controllers (any others won't be used), receives joystick index and logs a few messages.
	void OnControllerRemoved(int index); //Callback for whenever a game controller is plugged out, receives joystick index

	PlayerController* GetPlayerControllerWithJoystickIndex(int joystickIndex_) const; //Returns player with joysticks id (internal value) == joystickIndex_

	PlayerController* GetPlayerController(int index) const; //Returns player at array position index

private:
	const char* GetControllerTypeAsString(SDL_GameControllerType type);

private:
	char* droppedFilePath = nullptr;					  // SDL_DropEvent. Stores the path of a file when it is drag&dropped into the engine.
	KeyState keyboard[SDL_NUM_SCANCODES] = {KS_IDLE};	  // Array that stores the 'KeyState' of every key in the keyboard. See KeyState for possible states.
	KeyState mouseButtons[NUM_MOUSE_BUTTONS] = {KS_IDLE}; // Same keystate, but for the mouse buttons.

	float mouseWheelMotion = 0;	 // Stores the increment registered by the mouse wheel on a frame.
	float2 mouseMotion = {0, 0}; // Stores de movement increment of the mouse position on a frame.
	float2 mouse = {0, 0};		 // Stores the mouse position.
	bool mouseWarped = false;	 // "Flag" to indicate that the mouse has been warped. When warping happens and it is set to true, the PreUpdate() will ignore the mouse motion for one frame.

	PlayerController* playerControllers[MAX_PLAYERS] = {nullptr}; //Only one will be used, but system is prepared to get inputs from two different Game Controllers
};