#include "GameController.h"

#include "GameObject.h"
#include "Modules/ModuleCamera.h"
#include "GameplaySystems.h"

#include "Math/float3x3.h"
#include "Geometry/frustum.h"

GENERATE_BODY_IMPL(GameController);

void GameController::Start() {

	speed = 50.f;
	rotationSpeedX = 10.f;
	rotationSpeedY = 10.f;
	focusDistance = 100.f;
	showWireframe = false;
	transitionFinished = false;
	Debug::Log("SCRIPT STARTED");

	gameCamera = GameplaySystems::GetGameObject("Game Camera");
	godCamera = GameplaySystems::GetGameObject("God Camera");
	staticCamera1 = GameplaySystems::GetGameObject("staticCamera1");
	staticCamera2 = GameplaySystems::GetGameObject("staticCamera2");
	staticCamera3 = GameplaySystems::GetGameObject("staticCamera3");
	staticCamera4 = GameplaySystems::GetGameObject("staticCamera4");

	player = GameplaySystems::GetGameObject("Fang");

	GameplaySystems::SetRenderCamera(gameCamera);
	godCameraActive = false;
	if (gameCamera && godCamera) godModeAvailable = true;
}

void GameController::Update() {
	if (!transitionFinished) {
		DoTransition();
	}

	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_G)) {
		if (godModeAvailable) {
			Debug::ToggleDebugMode();
			if (godCameraActive) {
				GameplaySystems::SetRenderCamera(gameCamera);
				godCameraActive = false;
			}
			else {
				GameplaySystems::SetRenderCamera(godCamera);
				godCameraActive = true;
			}
		}
	}

	// Static cameras
	if (!godCameraActive) {
		if (Input::GetKeyCode(Input::KEYCODE::KEY_0) && gameCamera) {
			GameplaySystems::SetRenderCamera(gameCamera);
			godCameraActive = false;
		}
		if (Input::GetKeyCode(Input::KEYCODE::KEY_1) && staticCamera1) {
			GameplaySystems::SetRenderCamera(staticCamera1);
			godCameraActive = false;
		}
		if (Input::GetKeyCode(Input::KEYCODE::KEY_2) && staticCamera2) {
			GameplaySystems::SetRenderCamera(staticCamera2);
			godCameraActive = false;
		}
		if (Input::GetKeyCode(Input::KEYCODE::KEY_3) && staticCamera3) {
			GameplaySystems::SetRenderCamera(staticCamera3);
			godCameraActive = false;
		}
		if (Input::GetKeyCode(Input::KEYCODE::KEY_4) && staticCamera4) {
			GameplaySystems::SetRenderCamera(staticCamera4);
			godCameraActive = false;
		}
	}

	// Godmode Controls
	ComponentCamera* camera = nullptr;
	ComponentTransform* transform = nullptr;
	if (godCamera) {
		transform = godCamera->GetComponent<ComponentTransform>();
		camera = godCamera->GetComponent<ComponentCamera>();
	}
	if (!transform) return;
	if (!camera) return;

	if (godCameraActive) {
		// Movement
		// --- Forward
		if (Input::GetKeyCode(Input::KEYCODE::KEY_UP)) {
			transform->SetPosition(transform->GetPosition() + camera->GetFrustum()->Front().Normalized() * speed * Time::GetDeltaTime());
		}
		// --- Left
		if (Input::GetKeyCode(Input::KEYCODE::KEY_LEFT)) {
			transform->SetPosition(transform->GetPosition() + camera->GetFrustum()->WorldRight().Normalized() * -speed * Time::GetDeltaTime());
		}
		// --- Backward
		if (Input::GetKeyCode(Input::KEYCODE::KEY_DOWN)) {
			transform->SetPosition(transform->GetPosition() + camera->GetFrustum()->Front().Normalized() * -speed * Time::GetDeltaTime());
		}
		// --- Right
		if (Input::GetKeyCode(Input::KEYCODE::KEY_RIGHT)) {
			transform->SetPosition(transform->GetPosition() + camera->GetFrustum()->WorldRight().Normalized() * speed * Time::GetDeltaTime());
		}
		// --- Down
		if (Input::GetKeyCode(Input::KEYCODE::KEY_COMMA)) {
			transform->SetPosition(transform->GetPosition() + camera->GetFrustum()->Up().Normalized() * -speed * Time::GetDeltaTime());
		}
		if (Input::GetKeyCode(Input::KEYCODE::KEY_PERIOD)) {
			transform->SetPosition(transform->GetPosition() + camera->GetFrustum()->Up().Normalized() * speed * Time::GetDeltaTime());
		}
		// Rotation
		if (Input::GetMouseButton(2)) { // TODO: Why a 2?! It should be a 3!
			if (Input::GetKeyCode(Input::KEYCODE::KEY_LALT)) {
				// --- Orbiting
				vec oldFocus = transform->GetPosition() + transform->GetLocalMatrix().Col3(2) * focusDistance;
				Rotate(Input::GetMouseMotion(), camera->GetFrustum(), transform);
				vec newFocus = transform->GetPosition() + transform->GetLocalMatrix().Col3(2) * focusDistance;
				transform->SetPosition(transform->GetPosition() + (oldFocus - newFocus));
			}
			else {
				// --- Panning
				Rotate(Input::GetMouseMotion(), camera->GetFrustum(), transform);
			}
		}

		// Func
		// --- Show/Hide DebugDraw
		if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_Z)) {
			Debug::ToggleDebugDraw();
		}
		// --- Show/Hide Wireframe
		if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_X)) {
			if (showWireframe) {
				Debug::UpdateShadingMode("Shaded");
			}
			else {
				Debug::UpdateShadingMode("Wireframe");
			}
			showWireframe = !showWireframe;
		}
		// --- Show/Hide Quadtree
		if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_C)) {
			Debug::ToggleDrawQuadtree();
		}
		// --- Show/Hide Bounding Boxes
		if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_V)) {
			//Debug::ToggleDrawBBoxes(); //TODO: Disabled until better level building
		}
		// --- Show/Hide Animation Bones
		if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_B)) {
			Debug::ToggleDrawAnimationBones();
		}
		// --- Show/Hide All Light Gizmos
		if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_N)) {
			Debug::ToggleDrawLightGizmos();
		}
		// --- Show/Hide All Camera Frustums
		if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_M)) {
			Debug::ToggleDrawCameraFrustums();
		}
		// --- Show/Hide Skybox
		if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_K)) {
			ComponentSkyBox* skybox = gameCamera->GetComponent<ComponentSkyBox>();
			if (skybox->IsActive()) {
				skybox->Disable();
			}
			else {
				skybox->Enable();
			}
		}
	}
}

void GameController::Rotate(float2 mouseMotion, Frustum* frustum, ComponentTransform* transform) {
	Quat yIncrement = Quat::RotateY(-mouseMotion.x * rotationSpeedY * DEGTORAD * Time::GetDeltaTime());
	Quat xIncrement = Quat::RotateAxisAngle(frustum->WorldRight().Normalized(), -mouseMotion.y * rotationSpeedX * DEGTORAD * Time::GetDeltaTime());
	transform->SetRotation(yIncrement * xIncrement * transform->GetRotation());
}

void GameController::DoTransition()
{
	if (player != nullptr) {
		float3 finalPosition = float3(-164, 478, 449);
		float3 currentPosition = gameCamera->GetComponent<ComponentTransform>()->GetPosition();

		if (currentPosition.x > finalPosition.x) {
			currentPosition.x -= transitionSpeed * Time::GetDeltaTime();
			gameCamera->GetComponent<ComponentTransform>()->SetPosition(currentPosition);
		}
		else {
			transitionFinished = true;
			gameCamera->GetComponent<ComponentTransform>()->SetPosition(finalPosition);
		}
	}
}
