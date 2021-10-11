#include "PanelResource.h"

#include "Resources/Resource.h"
#include "Application.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleUserInterface.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleResources.h"

#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "IconsForkAwesome.h"

#include "Utils/Leaks.h"

PanelResource::PanelResource()
	: Panel("Resource", true) {}

void PanelResource::Update() {
	ImGui::SetNextWindowDockID(App->editor->dockRightId, ImGuiCond_FirstUseEver);
	std::string windowName = std::string(ICON_FA_FILE " ") + name;
	if (ImGui::Begin(windowName.c_str(), &enabled)) {
		Resource* selected = App->resources->GetResource<Resource>(App->editor->selectedResource);
		if (selected != nullptr) {
			ImGui::TextUnformatted("Id:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%llu", selected->GetId());

			ImGui::Separator();

			// Load button
			ImGui::TextColored(App->editor->titleColor, "Configuration");
			if (ImGui::Button("Load")) {
				App->resources->UnloadResource(selected);
				App->resources->LoadResource(selected);
			}

			ImGui::Separator();

			selected->OnEditorUpdate();
		}
	}
	ImGui::End();
}