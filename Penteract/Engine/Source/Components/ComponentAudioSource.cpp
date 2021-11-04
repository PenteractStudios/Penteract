#include "ComponentAudioSource.h"

#include "Globals.h"
#include "Application.h"
#include "GameObject.h"
#include "Modules/ModuleAudio.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleTime.h"
#include "Resources/ResourceAudioClip.h"
#include "Panels/PanelAudioMixer.h"
#include "Utils/ImGuiUtils.h"

#include "AL/al.h"
#include "debugdraw.h"
#include "Math/float3.h"
#include "IconsFontAwesome5.h"

#include "Utils/Leaks.h"

#define JSON_TAG_PITCH "Pitch"
#define JSON_TAG_GAIN "Gain"
#define JSON_TAG_MUTE "Mute"
#define JSON_TAG_LOOPING "Looping"
#define JSON_TAG_PLAY_ON_AWAKE "PlayOnAwake"
#define JSON_TAG_AUDIO_CLIP_ID "AudioClipId"
#define JSON_TAG_SPATIAL_BLEND "SpatialBlend"
#define JSON_TAG_SOURCE_TYPE "SourceType"
#define JSON_TAG_INNER_ANGLE "InnerAngle"
#define JSON_TAG_OUTER_ANGLE "OuterAngle"
#define JSON_TAG_OUTER_GAIN "OuterGain"
#define JSON_TAG_ROLLOFF_FACTOR "RolloffFactor"
#define JSON_TAG_REFERENCE_DISTANCE "ReferenceDistance"
#define JSON_TAG_MAX_DISTANCE "MaxDistance"
#define JSON_TAG_MUSIC "IsMusic"

ComponentAudioSource::~ComponentAudioSource() {
	Stop();
}

void ComponentAudioSource::Init() {
	isStarted = false;
	if (isMusic) {
		gainMultiplier = App->audio->GetGainMusicChannel();
	} else {
		gainMultiplier = App->audio->GetGainSFXChannel();
	}
	UpdateAudioSource();
}

void ComponentAudioSource::Update() {
	if (!isStarted && App->time->HasGameStarted() && playOnAwake) {
		Play();
		isStarted = true;
	}

	UpdateAudioSource();
	if (sourceId != 0 && (!IsPlaying() && !IsPaused())) {
		Stop();
	}
}

void ComponentAudioSource::DrawGizmos() {
	if (IsActive() && drawGizmos) {
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
	ImGui::Checkbox("Draw Gizmos", &drawGizmos);
	ImGui::Separator();
	ImGui::PushItemWidth(200);

	ImGui::TextColored(App->editor->titleColor, "Player");
	std::string play = std::string(" " ICON_FA_PLAY " ");
	std::string pause = std::string(" " ICON_FA_PAUSE " ");
	std::string stop = std::string(" " ICON_FA_STOP " ");
	if (ImGui::Button(play.c_str())) {
		Play();
	}
	ImGui::SameLine();
	if (ImGui::Button(pause.c_str())) {
		Pause();
	}
	ImGui::SameLine();
	if (ImGui::Button(stop.c_str())) {
		Stop();
	}
	ImGui::NewLine();

	ImGui::TextColored(App->editor->titleColor, "Channel Settings");
	int channel = isMusic;
	if (ImGui::RadioButton("Music", &channel, 1)) {
		isMusic = true;
		gainMultiplier = App->audio->GetGainMusicChannel();
		if (!mute && sourceId) {
			alSourcef(sourceId, AL_GAIN, gain * gainMultiplier);
		}
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("SFX", &channel, 0)) {
		isMusic = false;
		gainMultiplier = App->audio->GetGainSFXChannel();
		if (!mute && sourceId) {
			alSourcef(sourceId, AL_GAIN, gain * gainMultiplier);
		}
	}
	ImGui::NewLine();

	ImGui::TextColored(App->editor->titleColor, "General Settings");
	ImGui::ResourceSlot<ResourceAudioClip>("AudioClip", &audioClipId, [this]() { Stop(); });

	if (ImGui::Checkbox("Mute", &mute)) {
		if (sourceId) {
			if (mute) {
				alSourcef(sourceId, AL_GAIN, 0.0f);
			} else {
				alSourcef(sourceId, AL_GAIN, gain * gainMultiplier);
			}
		}
	}
	if (ImGui::Checkbox("Loop", &loop)) {
		if (sourceId) alSourcef(sourceId, AL_LOOPING, loop);
	}
	if (ImGui::DragFloat("Gain", &gain, App->editor->dragSpeed3f, 0, 1)) {
		if (!mute && sourceId) {
			alSourcef(sourceId, AL_GAIN, gain * gainMultiplier);
		}
	}
	if (ImGui::DragFloat("Pitch", &pitch, App->editor->dragSpeed3f, 0.5, 2)) {
		if (sourceId) alSourcef(sourceId, AL_PITCH, pitch);
	}
	ImGui::Checkbox("Play On Awake", &playOnAwake);
	ImGui::NewLine();

	ImGui::TextColored(App->editor->titleColor, "Position Settings");
	ImGui::Text("Spatial Blend");
	ImGui::SameLine();
	if (ImGui::RadioButton("2D", &spatialBlend, 0)) {
		if (sourceId) {
			alSourcei(sourceId, AL_SOURCE_RELATIVE, AL_TRUE);
			alSource3f(sourceId, AL_POSITION, 0.0f, 0.0f, 0.0f);
			alSource3f(sourceId, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
		}
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("3D", &spatialBlend, 1)) {
		if (sourceId) {
			alSourcei(sourceId, AL_SOURCE_RELATIVE, AL_FALSE);
			alSourcefv(sourceId, AL_POSITION, position.ptr());
		}
	}
	if (spatialBlend) { // 3D
		const char* sourceTypes[] = {"Omnidirectional", "Directional"};
		const char* sourceTypesCurrent = sourceTypes[sourceType];
		if (ImGui::BeginCombo("Source Type", sourceTypesCurrent)) {
			for (int n = 0; n < IM_ARRAYSIZE(sourceTypes); ++n) {
				bool isSelected = (sourceTypesCurrent == sourceTypes[n]);
				if (ImGui::Selectable(sourceTypes[n], isSelected)) {
					sourceType = n;
					if (sourceId) {
						if (sourceType) {
							alSourcefv(sourceId, AL_DIRECTION, direction.ptr());
						} else {
							alSourcef(sourceId, AL_CONE_INNER_ANGLE, 360);
							alSource3f(sourceId, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
						}
					}
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (sourceType) { // Directional
			ImGui::Indent();
			if (ImGui::DragFloat("Inner Angle", &innerAngle, 1.f, 0.0f, outerAngle)) {
				if (sourceId) alSourcef(sourceId, AL_CONE_INNER_ANGLE, innerAngle);
			}
			if (ImGui::DragFloat("Outer Angle", &outerAngle, 1.f, 0.0f, 360.f)) {
				if (sourceId) alSourcef(sourceId, AL_CONE_OUTER_ANGLE, outerAngle);
			}
			if (ImGui::DragFloat("Outer Gain", &outerGain, App->editor->dragSpeed2f, 0.f, 1.f)) {
				if (sourceId) alSourcef(sourceId, AL_CONE_OUTER_GAIN, outerGain);
			}
			ImGui::Unindent();
		}

		if (ImGui::DragFloat("Roll Off Factor", &rollOffFactor, 1.f, 0.0f, inf)) {
			if (sourceId) alSourcef(sourceId, AL_ROLLOFF_FACTOR, rollOffFactor);
		}
		if (ImGui::DragFloat("Reference Distance", &referenceDistance, 1.f, 0.0f, inf)) {
			if (sourceId) alSourcef(sourceId, AL_REFERENCE_DISTANCE, referenceDistance);
		}
		if (ImGui::DragFloat("Max Distance", &maxDistance, 1.f, 0.0f, inf)) {
			if (sourceId) alSourcef(sourceId, AL_MAX_DISTANCE, maxDistance);
		}
	}

	ImGui::NewLine();
	ImGui::PopItemWidth();
}

bool ComponentAudioSource::UpdateSourceParameters() {
	ResourceAudioClip* audioResource = App->resources->GetResource<ResourceAudioClip>(audioClipId);
	if (audioResource == nullptr) return false;
	if (audioResource->ALbuffer == 0) return false;

	if (!sourceId) return false;

	alSourcef(sourceId, AL_PITCH, pitch);
	alSourcei(sourceId, AL_LOOPING, loop);
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
		alSourcef(sourceId, AL_GAIN, gain * gainMultiplier);
	}
	alSourcef(sourceId, AL_ROLLOFF_FACTOR, rollOffFactor);
	alSourcef(sourceId, AL_REFERENCE_DISTANCE, referenceDistance);
	alSourcef(sourceId, AL_MAX_DISTANCE, maxDistance);

	return true;
}

void ComponentAudioSource::Play() {
	if (IsActive()) {
		if (!IsPlaying() && !IsPaused()) {
			Stop();
			sourceId = App->audio->GetAvailableSource();
			if (sourceId) UpdateSourceParameters();
		}
		if (sourceId) alSourcePlay(sourceId);
	}
}

void ComponentAudioSource::Stop() {
	if (sourceId) {
		alSourceStop(sourceId);
		sourceId = 0;
	}
}

void ComponentAudioSource::Pause() const {
	if (IsPlaying()) {
		alSourcePause(sourceId);
	}
}

bool ComponentAudioSource::IsPlaying() const {
	if (!sourceId) return false;
	ALint state;
	alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}

bool ComponentAudioSource::IsPaused() const {
	if (!sourceId) return false;
	ALint state;
	alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
	return (state == AL_PAUSED);
}

bool ComponentAudioSource::IsStopped() const {
	if (!sourceId) return true;
	ALint state;
	alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
	return (state == AL_STOPPED);
}

void ComponentAudioSource::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_PITCH] = pitch;
	jComponent[JSON_TAG_GAIN] = gain;
	jComponent[JSON_TAG_MUTE] = mute;
	jComponent[JSON_TAG_LOOPING] = loop;
	jComponent[JSON_TAG_PLAY_ON_AWAKE] = playOnAwake;
	jComponent[JSON_TAG_AUDIO_CLIP_ID] = audioClipId;
	jComponent[JSON_TAG_SPATIAL_BLEND] = spatialBlend;
	jComponent[JSON_TAG_SOURCE_TYPE] = sourceType;
	jComponent[JSON_TAG_INNER_ANGLE] = innerAngle;
	jComponent[JSON_TAG_OUTER_ANGLE] = outerAngle;
	jComponent[JSON_TAG_OUTER_GAIN] = outerGain;
	jComponent[JSON_TAG_ROLLOFF_FACTOR] = rollOffFactor;
	jComponent[JSON_TAG_REFERENCE_DISTANCE] = referenceDistance;
	jComponent[JSON_TAG_MAX_DISTANCE] = maxDistance;
	jComponent[JSON_TAG_MUSIC] = isMusic;
}

void ComponentAudioSource::Load(JsonValue jComponent) {
	pitch = jComponent[JSON_TAG_PITCH];
	gain = jComponent[JSON_TAG_GAIN];
	mute = jComponent[JSON_TAG_MUTE];
	loop = jComponent[JSON_TAG_LOOPING];
	playOnAwake = jComponent[JSON_TAG_PLAY_ON_AWAKE];
	audioClipId = jComponent[JSON_TAG_AUDIO_CLIP_ID];
	spatialBlend = jComponent[JSON_TAG_SPATIAL_BLEND];
	sourceType = jComponent[JSON_TAG_SOURCE_TYPE];
	innerAngle = jComponent[JSON_TAG_INNER_ANGLE];
	outerAngle = jComponent[JSON_TAG_OUTER_ANGLE];
	outerGain = jComponent[JSON_TAG_OUTER_GAIN];
	rollOffFactor = jComponent[JSON_TAG_ROLLOFF_FACTOR];
	referenceDistance = jComponent[JSON_TAG_REFERENCE_DISTANCE];
	maxDistance = jComponent[JSON_TAG_MAX_DISTANCE];
	isMusic = jComponent[JSON_TAG_MUSIC];

	if (audioClipId) {
		App->resources->IncreaseReferenceCount(audioClipId);
	}
}

// --- GETTERS ---

bool ComponentAudioSource::GetMute() const {
	return mute;
}

bool ComponentAudioSource::GetLoop() const {
	return loop;
}

float ComponentAudioSource::GetGain() const {
	return gain;
}

float ComponentAudioSource::GetPitch() const {
	return pitch;
}

bool ComponentAudioSource::GetPlayOnAwake() const {
	return playOnAwake;
}

int ComponentAudioSource::GetSpatialBlend() const {
	return spatialBlend;
}

int ComponentAudioSource::GetSourceType() const {
	return sourceType;
}

float ComponentAudioSource::GetInnerAngle() const {
	return innerAngle;
}

float ComponentAudioSource::GetOuterAngle() const {
	return outerAngle;
}

float ComponentAudioSource::GetOuterGain() const {
	return outerGain;
}

float ComponentAudioSource::GetRollOffFactor() const {
	return rollOffFactor;
}

float ComponentAudioSource::GetReferenceDistance() const {
	return referenceDistance;
}

float ComponentAudioSource::GetMaxDistance() const {
	return maxDistance;
}

float ComponentAudioSource::GetGainMultiplier() const {
	return gainMultiplier;
}

float ComponentAudioSource::GetIsMusic() const {
	return isMusic;
}

// --- SETTERS ---

void ComponentAudioSource::SetMute(bool _mute) {
	mute = _mute;
	if (sourceId) {
		if (mute) {
			alSourcef(sourceId, AL_GAIN, 0.0f);
		} else {
			alSourcef(sourceId, AL_GAIN, gain * gainMultiplier);
		}
	}
}

void ComponentAudioSource::SetLoop(bool _loop) {
	loop = _loop;
	if (sourceId) alSourcei(sourceId, AL_LOOPING, loop);
}

void ComponentAudioSource::SetGain(float _gain) {
	gain = _gain;
	if (!mute && sourceId) {
		alSourcef(sourceId, AL_GAIN, gain * gainMultiplier);
	}
}

void ComponentAudioSource::SetPitch(float _pitch) {
	pitch = _pitch;
	if (sourceId) alSourcef(sourceId, AL_PITCH, pitch);
}

void ComponentAudioSource::SetPlayOnAwake(bool _playOnAwake) {
	playOnAwake = _playOnAwake;
}

void ComponentAudioSource::SetSpatialBlend(int _spatialBlend) {
	spatialBlend = _spatialBlend;
}

void ComponentAudioSource::SetSourceType(int _sourceType) {
	sourceType = _sourceType;
}

void ComponentAudioSource::SetInnerAngle(float _innerAngle) {
	innerAngle = _innerAngle;
	if (sourceId) alSourcef(sourceId, AL_CONE_INNER_ANGLE, innerAngle);
}

void ComponentAudioSource::SetOuterAngle(float _outerAngle) {
	outerAngle = _outerAngle;
	if (sourceId) alSourcef(sourceId, AL_CONE_OUTER_ANGLE, outerAngle);
}

void ComponentAudioSource::SetOuterGain(float _outerGain) {
	outerGain = _outerGain;
	if (sourceId) alSourcef(sourceId, AL_CONE_OUTER_GAIN, outerGain);
}

void ComponentAudioSource::SetRollOffFactor(float _rollOffFactor) {
	rollOffFactor = _rollOffFactor;
	if (sourceId) alSourcef(sourceId, AL_ROLLOFF_FACTOR, rollOffFactor);
}

void ComponentAudioSource::SetReferenceDistance(float _referenceDistance) {
	referenceDistance = _referenceDistance;
	if (sourceId) alSourcef(sourceId, AL_REFERENCE_DISTANCE, referenceDistance);
}

void ComponentAudioSource::SetMaxDistance(float _maxDistance) {
	maxDistance = _maxDistance;
	if (sourceId) alSourcef(sourceId, AL_MAX_DISTANCE, maxDistance);
}

void ComponentAudioSource::SetGainMultiplier(float _gainMultiplier) {
	gainMultiplier = _gainMultiplier;
	if (!mute && sourceId) {
		alSourcef(sourceId, AL_GAIN, gain * gainMultiplier);
	}
}

void ComponentAudioSource::SetIsMusic(float _isMusic) {
	isMusic = _isMusic;
	if (isMusic) {
		gainMultiplier = App->audio->GetGainMusicChannel();
	} else {
		gainMultiplier = App->audio->GetGainSFXChannel();
	}
	if (!mute && sourceId) {
		alSourcef(sourceId, AL_GAIN, gain * gainMultiplier);
	}
}
