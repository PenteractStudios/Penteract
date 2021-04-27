#include "ModuleUserInterface.h"

#include "GameObject.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentSelectable.h"
#include "Components/UI/ComponentEventSystem.h"
#include "Components/UI/ComponentEventSystem.h"
#include "Components/UI/ComponentCanvasRenderer.h"
#include "Components/ComponentBoundingBox2D.h"
#include "Application.h"
#include "ModuleFiles.h"
#include "ModuleEvents.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleInput.h"
#include "ModuleScene.h"
#include "UI/Interfaces/IPointerEnterHandler.h"
#include "UI/Interfaces/IPointerExitHandler.h"
#include "UI/Interfaces/IMouseClickHandler.h"
#include "Scene.h"
#include "TesseractEvent.h"
#include "Resources/ResourceFont.h"
#include "Utils/FileDialog.h"
#include "FileSystem/TextureImporter.h"
#include "Utils/Logging.h"

#include "GL/glew.h"

#include "Utils/Leaks.h"

bool ModuleUserInterface::Init() {
	return true;
}

bool ModuleUserInterface::Start() {
	CreateQuadVBO();
	App->events->AddObserverToEvent(TesseractEventType::MOUSE_CLICKED, this);
	App->events->AddObserverToEvent(TesseractEventType::MOUSE_RELEASED, this);
	return true;
}

UpdateStatus ModuleUserInterface::Update() {
	float2 mousePos = App->input->GetMousePosition(true);

	if (currentEvSys) {
		for (ComponentSelectable& selectable : App->scene->scene->selectableComponents) {
			ComponentBoundingBox2D* bb = selectable.GetOwner().GetComponent<ComponentBoundingBox2D>();

			if (!selectable.IsHovered()) {
				if (bb->GetWorldAABB().Contains(mousePos)) {
					selectable.OnPointerEnter();
				}
			} else {
				if (!bb->GetWorldAABB().Contains(mousePos)) {
					selectable.OnPointerExit();
				}
			}
		}
	}

	return UpdateStatus::CONTINUE;
}

bool ModuleUserInterface::CleanUp() {
	glDeleteBuffers(1, &quadVBO);
	return true;
}

void ModuleUserInterface::ReceiveEvent(TesseractEvent& e) {
	switch (e.type) {
	case TesseractEventType::MOUSE_CLICKED:
		if (!App->time->IsGameRunning()) break;
		if (currentEvSys != nullptr) {
			ComponentSelectable* lastHoveredSelectable = currentEvSys->GetCurrentlyHovered();
			if (lastHoveredSelectable != nullptr) {
				if (lastHoveredSelectable->IsInteractable()) {
					lastHoveredSelectable->TryToClickOn();
				}
			}
		}
		break;

	case TesseractEventType::MOUSE_RELEASED:
		if (!App->time->IsGameRunning()) break;
		if (currentEvSys != nullptr) {
			ComponentSelectable* lastHoveredSelectable = currentEvSys->GetCurrentlyHovered();
			if (lastHoveredSelectable != nullptr) {
				lastHoveredSelectable->OnDeselect();
			}
		}

		break;
	default:
		break;
	}
}

Character ModuleUserInterface::GetCharacter(UID font, char c) {
	ResourceFont* fontResource = App->resources->GetResource<ResourceFont>(font);

	if (fontResource == nullptr) {
		return Character();
	}
	return fontResource->characters[c];
}

void ModuleUserInterface::GetCharactersInString(UID font, const std::string& sentence, std::vector<Character>& charsInSentence) {
	ResourceFont* fontResource = App->resources->GetResource<ResourceFont>(font);

	if (fontResource == nullptr) {
		return;
	}

	for (std::string::const_iterator i = sentence.begin(); i != sentence.end(); ++i) {
		charsInSentence.push_back(fontResource->characters[*i]);
	}
}

void ModuleUserInterface::Render() {
	Scene* scene = App->scene->scene;
	if (scene != nullptr) {
		for (ComponentCanvasRenderer& canvasRenderer : scene->canvasRendererComponents) {
			if (canvasRenderer.GetOwner().IsActiveInHierarchy()) {
				canvasRenderer.Render(&canvasRenderer.GetOwner());
			}
		}
	}
}

unsigned int ModuleUserInterface::GetQuadVBO() {
	return quadVBO;
}

void ModuleUserInterface::CreateQuadVBO() {
	float buffer_data[] = {
		-0.5f,
		-0.5f,
		0.0f, //  v0 pos

		0.5f,
		-0.5f,
		0.0f, // v1 pos

		-0.5f,
		0.5f,
		0.0f, //  v2 pos

		0.5f,
		-0.5f,
		0.0f, //  v3 pos

		0.5f,
		0.5f,
		0.0f, // v4 pos

		-0.5f,
		0.5f,
		0.0f, //  v5 pos

		0.0f,
		0.0f, //  v0 texcoord

		1.0f,
		0.0f, //  v1 texcoord

		0.0f,
		1.0f, //  v2 texcoord

		1.0f,
		0.0f, //  v3 texcoord

		1.0f,
		1.0f, //  v4 texcoord

		0.0f,
		1.0f //  v5 texcoord
	};

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO); // set vbo active
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);
}

void ModuleUserInterface::SetCurrentEventSystem(ComponentEventSystem* ev) {
	currentEvSys = ev;
}

ComponentEventSystem* ModuleUserInterface::GetCurrentEventSystem() {
	return currentEvSys;
}

void ModuleUserInterface::ViewportResized() {
	for (ComponentCanvas& canvas : App->scene->scene->canvasComponents) {
		canvas.SetDirty(true);
	}
}
