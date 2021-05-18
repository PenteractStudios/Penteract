#include "ModuleTime.h"

#include "Globals.h"
#include "Application.h"
#include "Utils/Logging.h"
#include "FileSystem/SceneImporter.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleFiles.h"
#include "Modules/ModuleEvents.h"
#include "Modules/ModuleAudio.h"
#include "Scene.h"
#include "SDL_timer.h"
#include "Brofiler.h"
#include <ctime>

#include "Utils/Leaks.h"

ModuleTime::ModuleTime() {
	timer.Start();
}

bool ModuleTime::Start() {
	App->events->AddObserverToEvent(TesseractEventType::PRESSED_PAUSE, this);
	App->events->AddObserverToEvent(TesseractEventType::PRESSED_PLAY, this);
	App->events->AddObserverToEvent(TesseractEventType::PRESSED_RESUME, this);
	App->events->AddObserverToEvent(TesseractEventType::PRESSED_STEP, this);
	App->events->AddObserverToEvent(TesseractEventType::PRESSED_STOP, this);

	return true;
}

UpdateStatus ModuleTime::PreUpdate() {
	BROFILER_CATEGORY("ModuleTime - PreUpdate", Profiler::Color::Black)

	frameCount += 1;

	unsigned int realTime = timer.Read();
	realTimeDeltaMs = realTime - realTimeLastMs;
	realTimeLastMs = realTime;

	if (gameRunning) {
		timeDeltaMs = lroundf(realTimeDeltaMs * timeScale);
		timeLastMs += timeDeltaMs;
	} else if (gameStepOnce) {
		timeDeltaMs = stepDeltaTimeMs;
		timeLastMs += timeDeltaMs;

		gameStepOnce = false;
	} else {
		timeDeltaMs = 0;
	}

	logger->LogDeltaMS((float) realTimeDeltaMs);

	return UpdateStatus::CONTINUE;
}

void ModuleTime::ReceiveEvent(TesseractEvent& e) {
	switch (e.type) {
	case TesseractEventType::PRESSED_PLAY:
		StartGame();
		break;
	case TesseractEventType::PRESSED_STOP:
		StopGame();
		break;
	case TesseractEventType::PRESSED_RESUME:
		ResumeGame();
		break;
	case TesseractEventType::PRESSED_PAUSE:
		PauseGame();
		break;
	case TesseractEventType::PRESSED_STEP:
		StepGame();
		break;
	default:
		break;
	}
}

void ModuleTime::WaitForEndOfFrame() {
	BROFILER_CATEGORY("ModuleTime - WaitForEndOfFrame", Profiler::Color::Black)
	if (limitFramerate) {
		unsigned int realTimeMs = timer.Read();
		unsigned int frameMs = realTimeMs - realTimeLastMs;
		unsigned int minMs = 1000 / maxFps;
		if (frameMs < minMs) {
			SDL_Delay(minMs - frameMs);
		}
	}
}

UpdateStatus ModuleTime::ExitGame() {
	return UpdateStatus::STOP;
}

bool ModuleTime::HasGameStarted() const {
	return gameStarted;
}

bool ModuleTime::IsGameRunning() const {
	return gameRunning;
}

float ModuleTime::GetDeltaTime() const {
	return timeDeltaMs / 1000.0f;
}

float ModuleTime::GetRealTimeDeltaTime() const {
	return realTimeDeltaMs / 1000.0f;
}

float ModuleTime::GetFPS() const {
	return logger->fpsLog[logger->fpsLogIndex];
}

float ModuleTime::GetMS() const {
	return logger->msLog[logger->fpsLogIndex];
}

float ModuleTime::GetTimeSinceStartup() const {
	return timeLastMs / 1000.0f;
}

float ModuleTime::GetRealTimeSinceStartup() const {
	return realTimeLastMs / 1000.0f;
}

long long ModuleTime::GetCurrentTimestamp() const {
	return std::time(0);
}

unsigned int ModuleTime::GetFrameCount() const {
	return frameCount;
}

void ModuleTime::StartGame() {
	if (gameStarted) return;

#if !GAME
	SceneImporter::SaveScene(TEMP_SCENE_FILE_NAME);
	App->scene->scene->sceneLoaded = false;
#endif // !GAME

	if (App->camera->GetGameCamera()) {
		// Set the Game Camera as active
		App->camera->ChangeActiveCamera(App->camera->GetGameCamera(), true);
		App->camera->ChangeCullingCamera(App->camera->GetGameCamera(), true);
	} else {
		// TODO: Modal window. Warning - camera not set.
	}

	gameStarted = true;
	gameRunning = true;
}

void ModuleTime::StopGame() {
	if (!gameStarted) return;

	gameStarted = false;
	gameRunning = false;

	// Stop all audio sources
	App->audio->StopAllSources();

#if !GAME
	SceneImporter::LoadScene(TEMP_SCENE_FILE_NAME);
	App->files->Erase(TEMP_SCENE_FILE_NAME);
#endif

	// Reset to the Engine camera
	App->camera->ChangeActiveCamera(nullptr, false);
	App->camera->ChangeCullingCamera(nullptr, false);

	timeLastMs = 0;
}

void ModuleTime::PauseGame() {
	if (!gameStarted) return;
	if (!gameRunning) return;

	gameRunning = false;
}

void ModuleTime::ResumeGame() {
	if (!gameStarted) return;
	if (gameRunning) return;

	gameRunning = true;
}

void ModuleTime::StepGame() {
	if (!gameStarted) StartGame();
	if (gameRunning) PauseGame();

	gameStepOnce = true;
}