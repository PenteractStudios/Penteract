#include "GameplaySystems.h"

#include "GameObject.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/ComponentCamera.h"
#include "Application.h"
#include "Panels/PanelScene.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleEvents.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleWindow.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleAudio.h"
#include "Modules/ModuleUserInterface.h"
#include "Resources/ResourcePrefab.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceClip.h"
#include "FileSystem/SceneImporter.h"
#include "Utils/Logging.h"
#include "TesseractEvent.h"

#include "debugdraw.h"
#include "Geometry/Frustum.h"
#include "Geometry/LineSegment.h"
#include "SDL_events.h"

#include "Utils/Leaks.h"

// ----------- GAMEPLAY ------------ //

GameObject* GameplaySystems::GetGameObject(const char* name) {
	GameObject* root = App->scene->scene->root;
	return root->name == name ? root : root->FindDescendant(name);
}

GameObject* GameplaySystems::GetGameObject(UID id) {
	return App->scene->scene->GetGameObject(id);
}
// --------- Instantiating --------- //
GameObject* GameplaySystems::Instantiate(ResourcePrefab* prefab, float3 position, Quat rotation) {
	UID prefabId = prefab->BuildPrefab(App->scene->scene->root);
	GameObject* go = GameplaySystems::GetGameObject(prefabId);
	ComponentTransform* transform = go->GetComponent<ComponentTransform>();
	transform->SetGlobalRotation(rotation);
	transform->SetGlobalPosition(position);
	return go;
}

template<typename T>
T* GameplaySystems::GetResource(UID id) {
	return App->resources->GetResource<T>(id);
}

template TESSERACT_ENGINE_API ResourcePrefab* GameplaySystems::GetResource<ResourcePrefab>(UID id);
template TESSERACT_ENGINE_API ResourceMaterial* GameplaySystems::GetResource<ResourceMaterial>(UID id);
template TESSERACT_ENGINE_API ResourceClip* GameplaySystems::GetResource<ResourceClip>(UID id);

void GameplaySystems::SetRenderCamera(ComponentCamera* camera) {
	App->camera->ChangeActiveCamera(camera, true);
	App->camera->ChangeCullingCamera(camera, true);
}

void GameplaySystems::DestroyGameObject(GameObject* gameObject) {
	App->scene->DestroyGameObjectDeferred(gameObject);
}

// ------------- DEBUG ------------- //

void Debug::Log(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	LOG(fmt, args);
	va_end(args);
}

void Debug::ToggleDebugMode() {
	App->renderer->ToggleDebugMode();
}

void Debug::ToggleDebugDraw() {
	App->renderer->ToggleDebugDraw();
}

void Debug::ToggleDrawQuadtree() {
	App->renderer->ToggleDrawQuadtree();
}

void Debug::ToggleDrawBBoxes() {
	App->renderer->ToggleDrawBBoxes();
}

void Debug::ToggleDrawSkybox() {
	App->renderer->ToggleDrawSkybox();
}

void Debug::ToggleDrawAnimationBones() {
	App->renderer->ToggleDrawAnimationBones();
}

void Debug::ToggleDrawCameraFrustums() {
	App->renderer->ToggleDrawCameraFrustums();
}

void Debug::ToggleDrawLightGizmos() {
	App->renderer->ToggleDrawLightGizmos();
}
void Debug::ToggleDrawParticleGizmos() {
	App->renderer->ToggleDrawParticleGizmos();
}

void Debug::ToggleDrawLightFrustumGizmo() {
	App->renderer->ToggleDrawLightFrustumGizmo();
}

void Debug::UpdateShadingMode(const char* shadingMode) {
	App->renderer->UpdateShadingMode(shadingMode);
}

int Debug::GetTotalTriangles() {
	return App->scene->scene->GetTotalTriangles();
}

int Debug::GetCulledTriangles() {
	return App->renderer->GetCulledTriangles();
}

const float3 Debug::GetCameraDirection() {
	return App->camera->GetActiveCamera()->GetFrustum()->Front();
}

//Temporary hardcoded solution
bool Debug::IsGodModeOn() {
	return App->scene->godModeOn;
}

//Temporary hardcoded solution
void Debug::SetGodModeOn(bool godModeOn_) {
	App->scene->godModeOn = godModeOn_;
}

// ------------- TIME -------------- //

float Time::GetDeltaTime() {
	return App->time->GetDeltaTime();
}

float Time::GetFPS() {
	return App->time->GetFPS();
}

float Time::GetMS() {
	return App->time->GetMS();
}

void Time::PauseGame() {
	App->time->PauseGame();
}

void Time::ResumeGame() {
	App->time->ResumeGame();
}

// ------------- INPUT ------------- //
const int JOYSTICK_MAX_VALUE = 32767;

bool Input::GetMouseButtonDown(int button) {
	return App->input->GetMouseButtons()[button] == KS_DOWN;
}

bool Input::GetMouseButtonUp(int button) {
	return App->input->GetMouseButtons()[button] == KS_UP;
}

bool Input::GetMouseButtonRepeat(int button) {
	return App->input->GetMouseButtons()[button] == KS_REPEAT;
}

bool Input::GetMouseButton(int button) {
	return App->input->GetMouseButtons()[button];
}

const float2& Input::GetMouseMotion() {
	return App->input->GetMouseMotion();
}

const float3 Input::GetMouseWorldPosition() {
	float2 MousePositionNormalized = App->editor->panelScene.GetMousePosOnSceneNormalized();
	float4x4 Projection = App->camera->GetProjectionMatrix();
	float4x4 View = App->camera->GetViewMatrix();
	float4 ScreenPos = float4(MousePositionNormalized.x, MousePositionNormalized.y, 0.0f, 1.0f);
	float4x4 ProjView = Projection * View;
	ProjView.Inverse();
	float4 worldPos = ProjView * ScreenPos;
	return worldPos.xyz() / worldPos.w;
}

float2 Input::GetMousePosition() {
	return App->input->GetMousePosition(true);
}

const float2 Input::GetMousePositionNormalized() {
#if GAME
	float2 mouseInput = App->input->GetMousePosition(true);
	int width = App->window->GetWidth();
	int height = App->window->GetHeight();
	float2 mouseNormalized;
	mouseNormalized.x = -1 + 2 * std::max(-1.0f, std::min((mouseInput.x) / (width), 1.0f));
	mouseNormalized.y = 1 - 2 * std::max(-1.0f, std::min((mouseInput.y) / (height), 1.0f));
	return mouseNormalized;
#else
	return App->editor->panelScene.GetMousePosOnSceneNormalized();
#endif
}

bool Input::GetKeyCodeDown(KEYCODE keycode) {
	return App->input->GetKeyboard()[keycode] == KS_DOWN;
}

bool Input::GetKeyCodeUp(KEYCODE keycode) {
	return App->input->GetKeyboard()[keycode] == KS_UP;
}

bool Input::GetKeyCodeRepeat(KEYCODE keycode) {
	return App->input->GetKeyboard()[keycode] == KS_REPEAT;
}

bool Input::GetKeyCode(KEYCODE keycode) {
	return App->input->GetKeyboard()[keycode];
}

bool Input::GetControllerButtonDown(SDL_GameControllerButton button, int playerID) {
	PlayerController* player = App->input->GetPlayerController(playerID);
	return player ? player->gameControllerButtons[button] == KS_DOWN : false;
}

bool Input::GetControllerButtonUp(SDL_GameControllerButton button, int playerID) {
	PlayerController* player = App->input->GetPlayerController(playerID);
	return player ? player->gameControllerButtons[button] == KS_UP : false;
}

bool Input::GetControllerButton(SDL_GameControllerButton button, int playerID) {
	PlayerController* player = App->input->GetPlayerController(playerID);
	return player ? player->gameControllerButtons[button] == KS_REPEAT : false;
}

float Input::GetControllerAxisValue(SDL_GameControllerAxis axis, int playerID) {
	PlayerController* player = App->input->GetPlayerController(playerID);

	return player ? player->gameControllerAxises[axis] / JOYSTICK_MAX_VALUE : 0.0f;
}

bool Input::IsGamepadConnected(int index) {
	return App->input->GetPlayerController(index) != nullptr;
}

// --------- SCENE MANAGER --------- //

void SceneManager::ChangeScene(UID sceneId) {
	TesseractEvent e(TesseractEventType::CHANGE_SCENE);
	e.Set<ChangeSceneStruct>(sceneId);
	App->events->AddEvent(e);
}

void SceneManager::ExitGame() {
	SDL_Event event;
	event.type = SDL_QUIT;
	SDL_PushEvent(&event);
}

GameObject* Physics::Raycast(const float3& start, const float3& end, const int mask) {
	LineSegment ray = LineSegment(start, end);

	Scene* scene = App->scene->scene;

	GameObject* closestGo = nullptr;
	float closestNear = FLT_MAX;
	float closestFar = FLT_MIN;

	for (GameObject& go : scene->gameObjects) {
		if ((go.GetMask().bitMask & mask) == 0) continue;
		ComponentBoundingBox* componentBBox = go.GetComponent<ComponentBoundingBox>();
		if (componentBBox == nullptr) continue;
		const AABB& bbox = componentBBox->GetWorldAABB();

		float dNear, dFar;

		if (ray.Intersects(bbox, dNear, dFar)) {
			if (closestGo == nullptr) {
				closestGo = &go;
			} else {
				if (dNear < closestFar) {
					closestGo = &go;
				}
			}
		}
	}

	return closestGo;
}

void Physics::CreateRigidbody(Component* collider)
{
	switch (collider->GetType()) {
	case ComponentType::BOX_COLLIDER:
		App->physics->CreateBoxRigidbody((ComponentBoxCollider*)collider);
		break;
	case ComponentType::SPHERE_COLLIDER:
		App->physics->CreateSphereRigidbody((ComponentSphereCollider*)collider);
		break;
	case ComponentType::CAPSULE_COLLIDER:
		App->physics->CreateCapsuleRigidbody((ComponentCapsuleCollider*)collider);
		break;
	default:
		break;
	}
}

void Physics::UpdateRigidbody(Component* collider)
{
	switch (collider->GetType()) {
	case ComponentType::BOX_COLLIDER:
		App->physics->UpdateBoxRigidbody((ComponentBoxCollider*)collider);
		break;
	case ComponentType::SPHERE_COLLIDER:
		App->physics->UpdateSphereRigidbody((ComponentSphereCollider*)collider);
		break;
	case ComponentType::CAPSULE_COLLIDER:
		App->physics->UpdateCapsuleRigidbody((ComponentCapsuleCollider*)collider);
		break;
	default:
		break;
	}
}

void Physics::RemoveRigidbody(Component* collider)
{
	switch (collider->GetType()) {
	case ComponentType::BOX_COLLIDER:
		App->physics->RemoveBoxRigidbody((ComponentBoxCollider*)collider);
		break;
	case ComponentType::SPHERE_COLLIDER:
		App->physics->RemoveSphereRigidbody((ComponentSphereCollider*)collider);
		break;
	case ComponentType::CAPSULE_COLLIDER:
		App->physics->RemoveCapsuleRigidbody((ComponentCapsuleCollider*)collider);
		break;
	default:
		break;
	}
}

float3 Colors::Red() {
	return dd::colors::Red;
}

float3 Colors::White() {
	return dd::colors::White;
}

float3 Colors::Blue() {
	return dd::colors::Blue;
}

float3 Colors::Orange() {
	return dd::colors::Orange;
}

float3 Colors::Green() {
	return dd::colors::Green;
}

// --------- Screen --------- //

void Screen::SetWindowMode(WindowMode mode) {
	App->window->SetWindowMode(mode);
}

void Screen::SetCurrentDisplayMode(unsigned index) {
	App->window->SetCurrentDisplayMode(index);
}

void Screen::SetSize(int width, int height) {
	App->window->SetSize(width, height);
}

void Screen::SetBrightness(float brightness) {
	App->window->SetBrightness(brightness);
}

WindowMode Screen::GetWindowMode() {
	return App->window->GetWindowMode();
}

bool Screen::GetMaximized() {
	return App->window->GetMaximized();
}

unsigned Screen::GetCurrentDisplayMode() {
	return App->window->GetCurrentDisplayMode();
}

unsigned Screen::GetNumDisplayModes() {
	return App->window->GetNumDisplayModes();
}

Screen::DisplayMode Screen::GetDisplayMode(unsigned index) {
	return Screen::DisplayMode(App->window->GetDisplayMode(index));
}

int Screen::GetWidth() {
	return App->window->GetWidth();
}

int Screen::GetHeight() {
	return App->window->GetHeight();
}

float Screen::GetBrightness() {
	return App->window->GetBrightness();
}

float2 Screen::GetResolution() {
	return float2(static_cast<float>(App->window->GetWidth()), static_cast<float>(App->window->GetHeight()));
}

const bool Screen::IsVSyncActive() {
	return App->time->vsync;
}

void Screen::SetVSync(bool value) {
	App->time->SetVSync(value);
}

void Screen::SetMSAAActive(bool value) {
	App->renderer->msaaActive = value;
	App->renderer->UpdateFramebuffers();
}

void Screen::SetMSAAType(MSAA_SAMPLES_TYPE value) {
	App->renderer->msaaSampleType = value;
	App->renderer->UpdateFramebuffers();
}

const bool Screen::IsMSAAActive() {
	return App->renderer->msaaActive;
}

const MSAA_SAMPLES_TYPE Screen::GetMSAAType() {
	return App->renderer->msaaSampleType;
}

const float Screen::GetBloomThreshold() {
	return App->renderer->bloomThreshold;
}

void Screen::SetBloomThreshold(float value) {
	App->renderer->bloomThreshold = value;
}

// --------- Camera --------- //

bool Camera::CheckObjectInsideFrustum(GameObject* gameObject) {
	return App->renderer->ObjectInsideFrustum(gameObject);
}

// --------- Audio --------- //

void Audio::StopAllSources() {
	App->audio->StopAllSources();
}

ComponentEventSystem* UserInterface::GetCurrentEventSystem() {
	return App->userInterface->GetCurrentEventSystem();
}
