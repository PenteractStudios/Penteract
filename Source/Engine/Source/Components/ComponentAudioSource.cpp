#include "ComponentAudioSource.h"

#include "GameObject.h"
#include "Globals.h"
#include "Application.h"
#include "Modules/ModuleAudio.h"
#include "Modules/ModuleEditor.h"
#include "Resources/ResourceAudioClip.h"
#include "Utils/Logging.h"
#include "Utils/ImGuiUtils.h"

#include "AL/al.h"
#include "debugdraw.h"
#include "Math/float3.h"
#include "imgui.h"

#include "Utils/Leaks.h"

#define JSON_TAG_PITCH "Pitch"
#define JSON_TAG_GAIN "Gain"
#define JSON_TAG_MUTE "Mute"
#define JSON_TAG_LOOPING "Looping"
#define JSON_TAG_AUDIO_CLIP_ID "AudioClipId"
#define JSON_TAG_SPATIAL_BLEND "SpatialBlend"
#define JSON_TAG_SOURCE_TYPE "SourceType"
#define JSON_TAG_INNER_ANGLE "InnerAngle"
#define JSON_TAG_OUTER_ANGLE "OuterAngle"
#define JSON_TAG_OUTER_GAIN "OuterGain"

void ComponentAudioSource::Init() {
	UpdateAudioSource();
}

void ComponentAudioSource::Update() {
	UpdateAudioSource();
}

void ComponentAudioSource::DrawGizmos() {
	if (IsActiveInHierarchy() && drawGizmos) {
		if (spatialBlend && sourceType) {
			dd::cone(position, direction, dd::colors::White, 1.0f, 0.0f);
		} else {
			dd::sphere(position, dd::colors::White, 2.f);
		}
	}
}

void ComponentAudioSource::UpdateAudioSource() {
	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
	float3 tempPos = transform->GetGlobalPosition();
	float3 tempDir = transform->GetGlobalRotation() * float3::unitZ;
	if (!tempPos.Equals(position) || !tempDir.Equals(direction)) {
		position = tempPos;
		direction = tempDir;

		if (!sourceId) return;

		if (spatialBlend) {
			alSourcefv(sourceId, AL_POSITION, position.ptr());
			if (sourceType) {
				alSourcefv(sourceId, AL_DIRECTION, direction.ptr());
			}
		}
	}
}

void ComponentAudioSource::OnEditorUpdate() {
	bool active = IsActive();
	if (ImGui::Checkbox("Active", &active)) {
		active ? Enable() : Disable();
	}
	ImGui::Separator();
	ImGui::Checkbox("Draw Gizmos", &drawGizmos);
	ImGui::Separator();
	ImGui::TextColored(App->editor->titleColor, "General Settings");

	ImGui::ResourceSlot<ResourceAudioClip>("AudioClip", &audioClipId);

	if (ImGui::Checkbox("Mute", &mute)) {
		if (mute) {
			alSourcef(sourceId, AL_GAIN, 0.0f);
		} else {
			alSourcef(sourceId, AL_GAIN, gain);
		}
	}

	if (ImGui::Checkbox("Loop", &loopSound)) {
		alSourcef(sourceId, AL_LOOPING, loopSound);
	}
	if (ImGui::DragFloat("Gain", &gain, App->editor->dragSpeed3f, 0, 1)) {
		alSourcef(sourceId, AL_GAIN, gain);
	}
	if (ImGui::DragFloat("Pitch", &pitch, App->editor->dragSpeed3f, 0.5, 2)) {
		alSourcef(sourceId, AL_PITCH, pitch);
	}
	ImGui::Separator();
	ImGui::TextColored(App->editor->titleColor, "Position Settings");
	ImGui::Text("Spatial Blend");
	ImGui::SameLine();
	if (ImGui::RadioButton("2D", &spatialBlend, 0)) {
		alSourcei(sourceId, AL_SOURCE_RELATIVE, AL_TRUE);
		alSource3f(sourceId, AL_POSITION, 0.0f, 0.0f, 0.0f);
		alSource3f(sourceId, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("3D", &spatialBlend, 1)) {
		alSourcei(sourceId, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcefv(sourceId, AL_POSITION, position.ptr());
	}
	if (spatialBlend) { // 3D
		const char* sourceTypes[] = {"Omnidirectional", "Directional"};
		const char* sourceTypesCurrent = sourceTypes[sourceType];
		if (ImGui::BeginCombo("Source Type", sourceTypesCurrent)) {
			for (int n = 0; n < IM_ARRAYSIZE(sourceTypes); ++n) {
				bool isSelected = (sourceTypesCurrent == sourceTypes[n]);
				if (ImGui::Selectable(sourceTypes[n], isSelected)) {
					sourceType = n;
					if (sourceType) {
						alSourcefv(sourceId, AL_DIRECTION, direction.ptr());
					} else {
						alSourcef(sourceId, AL_CONE_INNER_ANGLE, 360);
						alSource3f(sourceId, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
					}
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (sourceType) { // Directional
			if (ImGui::DragFloat("Inner Angle", &innerAngle, 1.f, 0.0f, outerAngle)) {
				alSourcef(sourceId, AL_CONE_INNER_ANGLE, innerAngle);
			}
			if (ImGui::DragFloat("Outer Angle", &outerAngle, 1.f, 0.0f, 360.f)) {
				alSourcef(sourceId, AL_CONE_OUTER_ANGLE, outerAngle);
			}
			if (ImGui::DragFloat("Outer Gain", &outerGain, App->editor->dragSpeed2f, 0.f, 1.f)) {
				alSourcef(sourceId, AL_CONE_OUTER_GAIN, outerGain);
			}
		}
	}

	ImGui::Separator();

	if (ImGui::Button("Play")) {
		Play();
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause")) {
		Pause();
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop")) {
		Stop();
	}
}

void ComponentAudioSource::UpdateSourceParameters() const {
	ResourceAudioClip* audioResource = App->resources->GetResource<ResourceAudioClip>(audioClipId);
	if (audioResource == nullptr) return;

	alSourcef(sourceId, AL_PITCH, pitch);
	alSourcei(sourceId, AL_LOOPING, loopSound);
	alSourcei(sourceId, AL_BUFFER, audioResource->ALbuffer);

	if (!spatialBlend) {
		alSourcei(sourceId, AL_SOURCE_RELATIVE, AL_TRUE);
		alSource3f(sourceId, AL_POSITION, 0.0f, 0.0f, 0.0f);
		alSource3f(sourceId, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
	} else {
		alSourcei(sourceId, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcefv(sourceId, AL_POSITION, position.ptr());

		if (sourceType) {
			alSourcefv(sourceId, AL_DIRECTION, direction.ptr());
			alSourcef(sourceId, AL_CONE_INNER_ANGLE, innerAngle);
			alSourcef(sourceId, AL_CONE_OUTER_ANGLE, outerAngle);
			alSourcef(sourceId, AL_CONE_OUTER_GAIN, outerGain);

		} else {
			alSourcef(sourceId, AL_CONE_INNER_ANGLE, 360);
			alSource3f(sourceId, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
		}
	}
	if (mute) {
		alSourcef(sourceId, AL_GAIN, 0.0f);
	} else {
		alSourcef(sourceId, AL_GAIN, gain);
	}
}

void ComponentAudioSource::Play() {
	if (IsActive()) {
		if (!sourceId) {
			sourceId = App->audio->GetAvailableSource();
			UpdateSourceParameters();
		}
		alSourcePlay(sourceId);
	}
}

void ComponentAudioSource::Stop() {
	if (isPlaying()) {
		alSourceStop(sourceId);
		sourceId = 0;
	}
}

void ComponentAudioSource::Pause() const {
	if (isPlaying()) {
		alSourcePause(sourceId);
	}
}

bool ComponentAudioSource::isPlaying() const {
	ALint state;
	alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}

void ComponentAudioSource::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_PITCH] = pitch;
	jComponent[JSON_TAG_GAIN] = gain;
	jComponent[JSON_TAG_MUTE] = mute;
	jComponent[JSON_TAG_LOOPING] = loopSound;
	jComponent[JSON_TAG_AUDIO_CLIP_ID] = audioClipId;
	jComponent[JSON_TAG_SPATIAL_BLEND] = spatialBlend;
	jComponent[JSON_TAG_SOURCE_TYPE] = sourceType;
	jComponent[JSON_TAG_INNER_ANGLE] = innerAngle;
	jComponent[JSON_TAG_OUTER_ANGLE] = outerAngle;
	jComponent[JSON_TAG_OUTER_GAIN] = outerGain;
}

void ComponentAudioSource::Load(JsonValue jComponent) {
	pitch = jComponent[JSON_TAG_PITCH];
	gain = jComponent[JSON_TAG_GAIN];
	mute = jComponent[JSON_TAG_MUTE];
	loopSound = jComponent[JSON_TAG_LOOPING];
	audioClipId = jComponent[JSON_TAG_AUDIO_CLIP_ID];
	spatialBlend = jComponent[JSON_TAG_SPATIAL_BLEND];
	sourceType = jComponent[JSON_TAG_SOURCE_TYPE];
	innerAngle = jComponent[JSON_TAG_INNER_ANGLE];
	outerAngle = jComponent[JSON_TAG_OUTER_ANGLE];
	outerGain = jComponent[JSON_TAG_OUTER_GAIN];
}

void ComponentAudioSource::DuplicateComponent(GameObject& owner) {
	ComponentAudioSource* component = owner.CreateComponent<ComponentAudioSource>();
	component->pitch = this->pitch;
	component->gain = this->gain;
	component->mute = this->mute;
	component->loopSound = this->loopSound;
	component->audioClipId = this->audioClipId;
	component->spatialBlend = this->spatialBlend;
	component->sourceType = this->sourceType;
	component->innerAngle = this->innerAngle;
	component->outerAngle = this->outerAngle;
	component->outerGain = this->outerGain;
}