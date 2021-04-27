#include "StatsDisplayer.h"

#include "GameObject.h"
#include "Components/UI/ComponentText.h"
#include "GameplaySystems.h"

GENERATE_BODY_IMPL(StatsDisplayer);

void StatsDisplayer::Start() {
	player = GameplaySystems::GetGameObject("Fang");
	canvas = GameplaySystems::GetGameObject("CanvasStats");

	if (canvas != nullptr) {
		for (GameObject* child : canvas->GetChildren()) {
			if (child->name == "FPSText") {
				fpsText = child->GetComponent<ComponentText>();
			}
			else if (child->name == "TRIGText") {
				trigText = child->GetComponent<ComponentText>();
			}
			else if (child->name == "PlayerPositionText") {
				positionText = child->GetComponent<ComponentText>();
			}
			else if (child->name == "CameraDirectionText") {
				cameraText = child->GetComponent<ComponentText>();
			}
		}
	}
}

void StatsDisplayer::Update() {
	if (Input::GetKeyCodeDown(Input::KEYCODE::KEY_F2)) {
		panelActive = !panelActive;
	} 

	if (!canvas) return;
	if (panelActive) {
		canvas->Enable();
		if (fpsText != nullptr) {
			char title[80];
			sprintf_s(title, 80, "Framerate %.1fFPS. MS: %.1fms", Time::GetFPS(), Time::GetMS());
			fpsText->SetText(title);
		}

		if (trigText != nullptr) {
			char title[80];
			sprintf_s(title, 80, "Total triangles: %i. Culled Triangles: %i", Debug::GetTotalTriangles(), Debug::GetCulledTriangles());
			trigText->SetText(title);
		}

		if (player != nullptr && fpsText != nullptr) {
			char title[80];
			float3 position = player->GetComponent<ComponentTransform>()->GetPosition();
			sprintf_s(title, 80, "Player Position x:%.1f, y:%.1f, z:%.1f", position.x, position.y, position.z);
			positionText->SetText(title);
		}

		if (cameraText != nullptr) {
			char title[80];
			float3 front = Debug::GetCameraDirection();
			sprintf_s(title, 80, "Camera front: x:%.1f, y:%.1f, z:%.1f", front.x, front.y, front.z);
			cameraText->SetText(title);
		}
	}
	else {
		canvas->Disable();
	}
}