#include "ComponentAnimation.h"

#include "Application.h"
#include "GameObject.h"
#include "Animation/Transition.h"
#include "Animation/AnimationInterpolation.h"
#include "Animation/AnimationController.h"
#include "Resources/ResourceAnimation.h"
#include "Resources/ResourceClip.h"
#include "Components/ComponentTransform.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleEvents.h"
#include "Utils/UID.h"
#include "Utils/ImGuiUtils.h"
#include "Animation/StateMachineEnum.h"
#include "Animation/StateMachineManager.h"

#include <algorithm> // std::find

#include "Utils/Leaks.h"

#define JSON_TAG_LOOP "Controller"
#define JSON_TAG_ANIMATION_ID "AnimationId"
#define JSON_TAG_STATE_MACHINE_PRINCIPAL_ID "StateMachinePrincipalId"
#define JSON_TAG_STATE_MACHINE_SECONDARY_ID "StateMachineSecondaryId"
#define JSON_TAG_CLIP "Clip"

void ComponentAnimation::Update() {
	if (!App->time->IsGameRunning() || !IsActive()) {
		return;
	}
	if (!loadedResourceStateMachine) { //Checking if there is no state machine
		LoadResourceStateMachine(stateMachineResourceUIDPrincipal, StateMachineEnum::PRINCIPAL);
	}
	if (!loadedResourceStateMachineSecondary) { //Checking if there is no state machine
		LoadResourceStateMachine(stateMachineResourceUIDSecondary, StateMachineEnum::SECONDARY);
	}

	if (currentTimeStatesPrincipal.empty()) { //Checking if there is no state machine
		InitCurrentTimeStates(stateMachineResourceUIDPrincipal, StateMachineEnum::PRINCIPAL);
	}

	if (currentTimeStatesSecondary.empty()) { //Checking if there is no state machine
		InitCurrentTimeStates(stateMachineResourceUIDSecondary, StateMachineEnum::SECONDARY);
	}

	OnUpdate();
}

void ComponentAnimation::OnEditorUpdate() {
	if (ImGui::Checkbox("Active", &active)) {
		if (GetOwner().IsActive()) {
			if (active) {
				Enable();
			} else {
				Disable();
			}
		}
	}
	ImGui::Separator();

	ImGui::TextColored(App->editor->titleColor, "Animation");

	// Principal
	UID oldStateMachinePrincipalUID = stateMachineResourceUIDPrincipal;
	ImGui::ResourceSlot<ResourceStateMachine>("State Machine Principal", &stateMachineResourceUIDPrincipal);
	if (oldStateMachinePrincipalUID != stateMachineResourceUIDPrincipal) {
		ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUIDPrincipal);
		if (resourceStateMachine) {
			currentStatePrincipal = resourceStateMachine->initialState;
			loadedResourceStateMachine = true;
		} else {
			loadedResourceStateMachine = false;
		}
	}

	// Secondary
	UID oldStateMachineSecondaryUID = stateMachineResourceUIDSecondary;
	ImGui::ResourceSlot<ResourceStateMachine>("State Machine Secondary", &stateMachineResourceUIDSecondary);
	if (oldStateMachineSecondaryUID != stateMachineResourceUIDSecondary) {
		ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUIDSecondary);
		if (resourceStateMachine) {
			currentStateSecondary = resourceStateMachine->initialState;
			loadedResourceStateMachineSecondary = true;
		} else {
			loadedResourceStateMachineSecondary = false;
		}
	}
}

void ComponentAnimation::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_STATE_MACHINE_PRINCIPAL_ID] = stateMachineResourceUIDPrincipal;
	jComponent[JSON_TAG_STATE_MACHINE_SECONDARY_ID] = stateMachineResourceUIDSecondary;
}

void ComponentAnimation::Load(JsonValue jComponent) {
	stateMachineResourceUIDPrincipal = jComponent[JSON_TAG_STATE_MACHINE_PRINCIPAL_ID];
	stateMachineResourceUIDSecondary = jComponent[JSON_TAG_STATE_MACHINE_SECONDARY_ID];
	GameObject* rootBone = GetOwner().GetRootBone();
	GameObject owner = GetOwner();

	if (stateMachineResourceUIDPrincipal != 0) App->resources->IncreaseReferenceCount(stateMachineResourceUIDPrincipal);
	if (stateMachineResourceUIDSecondary != 0) App->resources->IncreaseReferenceCount(stateMachineResourceUIDSecondary);

	LoadResourceStateMachine(stateMachineResourceUIDPrincipal, StateMachineEnum::PRINCIPAL);
	LoadResourceStateMachine(stateMachineResourceUIDSecondary, StateMachineEnum::SECONDARY);

	InitCurrentTimeStates(stateMachineResourceUIDPrincipal, StateMachineEnum::PRINCIPAL);
	InitCurrentTimeStates(stateMachineResourceUIDSecondary, StateMachineEnum::SECONDARY);
}

void ComponentAnimation::OnUpdate() {
	// Update gameobjects matrix
	GameObject* rootBone = GetOwner().GetRootBone();

	UpdateAnimations(rootBone);

	if (loadedResourceStateMachine && animationInterpolationsPrincipal.empty()) {
		ResourceClip* currentClip = App->resources->GetResource<ResourceClip>(currentStatePrincipal.clipUid);
		if (!currentClip) {
			return;
		}
		currentTimeStatesPrincipal[currentStatePrincipal.id] += App->time->GetDeltaTime() * currentClip->speed;
	}
	if (loadedResourceStateMachineSecondary && currentStateSecondary.id != 0 && animationInterpolationsSecondary.empty()) {
		ResourceClip* currentClip = App->resources->GetResource<ResourceClip>(currentStateSecondary.clipUid);
		if (!currentClip) {
			return;
		}
		currentTimeStatesSecondary[currentStateSecondary.id] += (App->time->GetDeltaTime() * currentClip->speed);
	}
}

void ComponentAnimation::SendTrigger(const std::string& trigger) {
	StateMachineManager::SendTrigger(trigger, StateMachineEnum::PRINCIPAL, *this);
}
void ComponentAnimation::SendTriggerSecondary(const std::string& trigger) {
	if (loadedResourceStateMachine && currentStateSecondary.id == 0) {
		//For doing the interpolation between states correctly it is necessary to set the current state of the principal state machine to be the same as the second state machine current state
		currentStateSecondary = currentStatePrincipal;
		//Updating current time to the currentTimeStatesSecondary to sync the times between the current state machine of the first one and the second one
		currentTimeStatesSecondary[currentStateSecondary.id] = currentTimeStatesPrincipal[currentStatePrincipal.id];
	}

	StateMachineManager::SendTrigger(trigger, StateMachineEnum::SECONDARY, *this);
}

void ComponentAnimation::UpdateAnimations(GameObject* gameObject) {
	if (gameObject == nullptr) {
		return;
	}

	//find gameobject in hash
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	bool resetSecondaryStatemachine = false;

	bool result = StateMachineManager::UpdateAnimations(
		gameObject,
		GetOwner(),
		*this,
		position,
		rotation,
		resetSecondaryStatemachine);

	//If finishedTransition and currentStateSecondary equals to currentStatePrincipal we must reset the currentStateSecondary to "empty" state
	//See line 84 of StateMachineManager.cpp
	if (resetSecondaryStatemachine) {
		ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUIDSecondary);
		if (!resourceStateMachine) {
			return;
		}
		std::unordered_map<UID, State>::iterator it = resourceStateMachine->states.find(0); // Get "empty" state
		if (it != resourceStateMachine->states.end()) {
			currentStateSecondary = (*it).second;
		}
	}

	ComponentTransform* componentTransform = gameObject->GetComponent<ComponentTransform>();

	if (componentTransform && result) {
		componentTransform->SetPosition(position);
		componentTransform->SetRotation(rotation);
	}

	for (GameObject* child : gameObject->GetChildren()) {
		UpdateAnimations(child);
	}
}

void ComponentAnimation::LoadResourceStateMachine(UID stateMachineResourceUid, StateMachineEnum stateMachineEnum) {
	ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUid);

	if (resourceStateMachine) {
		switch (stateMachineEnum) {
		case StateMachineEnum::PRINCIPAL:
			currentStatePrincipal = State(resourceStateMachine->initialState);
			loadedResourceStateMachine = true;
			break;
		case StateMachineEnum::SECONDARY:
			currentStateSecondary = State(resourceStateMachine->initialState);
			loadedResourceStateMachineSecondary = true;
			break;
		}
	}
}

void ComponentAnimation::InitCurrentTimeStates(UID stateMachineResourceUid, StateMachineEnum stateMachineEnum) {
	ResourceStateMachine* resourceStateMachine = App->resources->GetResource<ResourceStateMachine>(stateMachineResourceUid);

	if (resourceStateMachine) {
		switch (stateMachineEnum) {
		case StateMachineEnum::PRINCIPAL:
			for (const auto& element : resourceStateMachine->states) {
				currentTimeStatesPrincipal.insert({element.first, 0.0f});
			}
			break;
		case StateMachineEnum::SECONDARY:
			for (const auto& element : resourceStateMachine->states) {
				currentTimeStatesSecondary.insert({element.first, 0.0f});

				//Filling the key events given that the secondary state machine has all of the states
				ResourceClip* clip = App->resources->GetResource<ResourceClip>(element.second.clipUid);
				if (clip) {
					listClipsKeyEvents.insert({element.second.clipUid, clip->keyEventClips});
					listClipsCurrentEventKeyFrames.insert({element.second.clipUid, 0});
				}
			}
			break;
		}
	}
}