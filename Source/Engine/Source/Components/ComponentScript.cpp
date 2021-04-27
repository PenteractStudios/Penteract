#include "ComponentScript.h"

#include "Application.h"
#include "Modules/ModuleProject.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleScene.h"
#include "Resources/ResourceScript.h"
#include "Utils/FileDialog.h"
#include "Utils/ImGuiUtils.h"
#include "GameObject.h"
#include "Script.h"

#include "imgui.h"

#include "Utils/Leaks.h"

#define JSON_TAG_SCRIPT "Script"
#define JSON_TAG_NAME "Name"

void ComponentScript::Update() {

	if (dirty) {
		ReloadScript();
	}

	if (App->time->HasGameStarted() && App->scene->sceneLoaded) {
		ResourceScript* resource = App->resources->GetResource<ResourceScript>(scriptID);
		if (resource != nullptr) {
			if (resource->script != nullptr) {
				resource->script->Update();
			}
		}
	}
}

void ComponentScript::OnStart() {
	ResourceScript* resource =  App->resources->GetResource<ResourceScript>(scriptID);
	if (resource != nullptr) {
		if (resource->script != nullptr) {
			resource->script->Start();
		}
	}
}

void ComponentScript::OnEditorUpdate() {
	bool active = IsActive();
	if (ImGui::Checkbox("Active", &active)) {
		active ? Enable() : Disable();
	}
	ImGui::Separator();
	ImGui::ResourceSlot<ResourceScript>("Script", &scriptID);

	static char name[1024] = "";
	ImGui::InputText("Script name", name, 1024);
	ImGui::SameLine();
	if (ImGui::Button("Create Script")) {
		App->project->CreateScript(std::string(name));
	}
}

void ComponentScript::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_SCRIPT] = scriptID;
}

void ComponentScript::Load(JsonValue jComponent) {
	scriptID = jComponent[JSON_TAG_SCRIPT];
	if (scriptID != 0) App->resources->IncreaseReferenceCount(scriptID);
}

void ComponentScript::DuplicateComponent(GameObject& owner) {
	ComponentScript* component = owner.CreateComponent<ComponentScript>();
	component->scriptID = this->scriptID;
}

UID ComponentScript::GetScriptID() const {
	return scriptID;
}

void ComponentScript::ReloadScript() {
	dirty = false;
	ResourceScript* script = App->resources->GetResource<ResourceScript>(scriptID);
	if(script) script->Load();
}