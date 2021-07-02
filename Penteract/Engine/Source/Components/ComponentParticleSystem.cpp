#include "ComponentParticleSystem.h"

#include "GameObject.h"
#include "Components/ComponentTransform.h"
#include "Components/UI/ComponentButton.h"
#include "Application.h"
#include "Modules/ModulePrograms.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleResources.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleUserInterface.h"
#include "Modules/ModulePhysics.h"
#include "Panels/PanelScene.h"
#include "Resources/ResourceTexture.h"
#include "Resources/ResourceShader.h"
#include "FileSystem/TextureImporter.h"
#include "FileSystem/JsonValue.h"
#include "Utils/Logging.h"
#include "Utils/ImGuiUtils.h"

#include "Math/float3x3.h"
#include "Math/TransformOps.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_color_gradient.h"
#include "GL/glew.h"
#include "debugdraw.h"
#include <random>

#include "Utils/Leaks.h"

// Gizmo
#define JSON_TAG_DRAW_GIZMO "DrawGizmo"

// Control
#define JSON_TAG_IS_PLAYING "IsPlaying"
#define JSON_TAG_START_DELAY "StartDelay"

// Particle System
#define JSON_TAG_DURATION "Duration"
#define JSON_TAG_LOOPING "Looping"
#define JSON_TAG_LIFE "Life"
#define JSON_TAG_VELOCITY "Velocity"
#define JSON_TAG_SCALE "Scale"
#define JSON_TAG_REVERSE_EFFECT "ReverseEffect"
#define JSON_TAG_REVERSE_DISTANCE "ReverseDistance"
#define JSON_TAG_MAX_PARTICLE "MaxParticle"

//Emision
#define JSON_TAG_ATTACH_EMITTER "AttachEmitter"

// Shape
#define JSON_TAG_EMITTER_TYPE "ParticleEmitterType"
#define JSON_TAG_CONE_RADIUS_UP "ConeRadiusUp"
#define JSON_TAG_CONE_RADIUS_DOWN "ConeRadiusDown"
#define JSON_TAG_RANDOM_CONE_RADIUS_UP "RandomConeRadiusUp"
#define JSON_TAG_RANDOM_CONE_RADIUS_DOWN "RandomConeRadiusDown"

// Size over Lifetime
#define JSON_TAG_SIZE_OVER_LIFETIME "SizeOverLifetime"
#define JSON_TAG_SCALE_FACTOR "ScaleFactor"

// Color over Lifetime
#define JSON_TAG_COLOR_OVER_LIFETIME "ColorOverLifetime"
#define JSON_TAG_NUMBER_COLORS "NumberColors"
#define JSON_TAG_GRADIENT_COLORS "GradientColors"

// Texture Sheet Animation
#define JSON_TAG_YTILES "Ytiles"
#define JSON_TAG_XTILES "Xtiles"
#define JSON_TAG_ANIMATION_SPEED "AnimationSpeed"
#define JSON_TAG_IS_RANDOM_FRAME "IsRandomFrame"

// Render
#define JSON_TAG_TEXTURE_TEXTURE_ID "TextureId"
#define JSON_TAG_BILLBOARD_TYPE "BillboardType"
#define JSON_TAG_PARTICLE_RENDER_MODE "ParticleRenderMode"
#define JSON_TAG_FLIP_TEXTURE "FlipTexture"

// Collision
#define JSON_TAG_HAS_COLLISION "HasCollision"
#define JSON_TAG_LAYER_INDEX "LayerIndex"

ComponentParticleSystem::~ComponentParticleSystem() {
	RELEASE(gradient);
}

void ComponentParticleSystem::OnEditorUpdate() {
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
	ImGui::Checkbox("Draw Gizmo", &drawGizmo);
	ImGui::Separator();

	ImGui::Indent();

	// Control
	if (ImGui::CollapsingHeader("Control Options")) {
		if (ImGui::Button("Play")) Play();
		if (isPlaying) {
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "Is Playing");
		}
		if (ImGui::Button("Stop")) Stop();

		if (ImGui::DragFloat("Start Delay", &startDelay, App->editor->dragSpeed2f, 0, inf)) {
			restDelayTime = startDelay;
		}
		if (startDelay > 0) ImGui::DragFloat("Rest Time", &restDelayTime, App->editor->dragSpeed2f, 0, inf, "%.3f", ImGuiSliderFlags_NoInput);
	}

	ImGui::NewLine();

	// General Particle System
	if (ImGui::CollapsingHeader("Particle System")) {
		ImGui::DragFloat("Duration", &duration, App->editor->dragSpeed2f, 0, inf);
		ImGui::Checkbox("Loop", &looping);
		if (looping) {
			Play();
		}
		ImGui::DragFloat("Start Life", &life, App->editor->dragSpeed2f, 0, inf);
		if (ImGui::DragFloat("Start Speed", &velocity, App->editor->dragSpeed2f, 0, inf)) {
			CreateParticles();
		}
		ImGui::DragFloat("Start Size", &scale, App->editor->dragSpeed2f, 0, inf);
		ImGui::Checkbox("Reverse Effect", &reverseEffect);
		if (reverseEffect) {
			ImGui::Indent();
			ImGui::DragFloat("Distance", &reverseDistance, App->editor->dragSpeed2f, 0, inf);
			ImGui::Unindent();
		}
		if (ImGui::DragScalar("MaxParticles", ImGuiDataType_U32, &maxParticles)) {
			if (maxParticles <= 2000) {
				CreateParticles();
			} else {
				LOG("Warning: Max particles: 2000")
			}
		}
	}

	// Emission
	if (ImGui::CollapsingHeader("Emission")) {
		ImGui::Checkbox("Attach to Emitter", &attachEmitter);
	}

	// Shape
	if (ImGui::CollapsingHeader("Shape")) {
		const char* emitterTypeCombo[] = {"Cone", "Sphere", "Hemisphere", "Donut", "Circle", "Rectangle"};
		const char* emitterTypeComboCurrent = emitterTypeCombo[(int) emitterType];
		ImGui::TextColored(App->editor->textColor, "Shape");
		if (ImGui::BeginCombo("##Shape", emitterTypeComboCurrent)) {
			for (int n = 0; n < IM_ARRAYSIZE(emitterTypeCombo); ++n) {
				bool isSelected = (emitterTypeComboCurrent == emitterTypeCombo[n]);
				if (ImGui::Selectable(emitterTypeCombo[n], isSelected)) {
					emitterType = (ParticleEmitterType) n;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (emitterType == ParticleEmitterType::CONE) {
			ImGui::Checkbox("Random##random_cone_radius_up", &randomConeRadiusUp);
			ImGui::SameLine();
			ImGui::DragFloat("Radius Up", &coneRadiusUp, App->editor->dragSpeed2f, 0, inf);

			ImGui::Checkbox("Random##random_cone_radius_down", &randomConeRadiusDown);
			ImGui::SameLine();
			ImGui::DragFloat("Radius Down", &coneRadiusDown, App->editor->dragSpeed2f, 0, inf);
		}
	}

	// Size over Lifetime
	if (ImGui::CollapsingHeader("Size over Lifetime")) {
		ImGui::Checkbox("##size_over_lifetime", &sizeOverLifetime);
		if (sizeOverLifetime) {
			ImGui::SameLine();
			ImGui::DragFloat("Scale Factor", &scaleFactor, App->editor->dragSpeed2f, -inf, inf);
		}
	}

	// Color over Lifetime
	if (ImGui::CollapsingHeader("Color over Lifetime")) {
		ImGui::Checkbox("##color_over_lifetime", &colorOverLifetime);
		if (colorOverLifetime) {
			ImGui::SameLine();
			ImGui::GradientEditor(gradient, draggingGradient, selectedGradient);
		}
	}

	// Texture Sheet Animation
	if (ImGui::CollapsingHeader("Texture Sheet Animation")) {
		ImGui::DragScalar("X Tiles", ImGuiDataType_U32, &Xtiles);
		ImGui::DragScalar("Y Tiles", ImGuiDataType_U32, &Ytiles);
		ImGui::DragFloat("Animation Speed", &animationSpeed, App->editor->dragSpeed2f, -inf, inf);
		ImGui::Checkbox("Random Frame", &isRandomFrame);
	}

	// Render
	if (ImGui::CollapsingHeader("Render")) {
		const char* billboardTypeCombo[] = {"Billboard", "Stretched Billboard", "Horitzontal Billboard ", "Vertical Billboard"};
		const char* billboardTypeComboCurrent = billboardTypeCombo[(int) billboardType];
		if (ImGui::BeginCombo("Bilboard Mode##", billboardTypeComboCurrent)) {
			for (int n = 0; n < IM_ARRAYSIZE(billboardTypeCombo); ++n) {
				bool isSelected = (billboardTypeComboCurrent == billboardTypeCombo[n]);
				if (ImGui::Selectable(billboardTypeCombo[n], isSelected)) {
					billboardType = (BillboardType) n;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		const char* renderModeCombo[] = {"Additive", "Transparent"};
		const char* renderModeComboCurrent = renderModeCombo[(int) renderMode];
		if (ImGui::BeginCombo("Render Mode##", renderModeComboCurrent)) {
			for (int n = 0; n < IM_ARRAYSIZE(renderModeCombo); ++n) {
				bool isSelected = (renderModeComboCurrent == renderModeCombo[n]);
				if (ImGui::Selectable(renderModeCombo[n], isSelected)) {
					renderMode = (ParticleRenderMode) n;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::ResourceSlot<ResourceTexture>("texture", &textureID);

		ImGui::NewLine();
		ImGui::Text("Flip: ");
		ImGui::SameLine();
		ImGui::Checkbox("X", &flipTexture[0]);
		ImGui::SameLine();
		ImGui::Checkbox("Y", &flipTexture[1]);
	}

	// Collision
	if (ImGui::CollapsingHeader("Collision")) {
		ImGui::Checkbox("##collision", &collision);
		if (collision) {
			ImGui::Indent();

			// World Layers combo box
			const char* layerTypeItems[] = {"No Collision", "Event Triggers", "World Elements", "Player", "Enemy", "Bullet", "Bullet Enemy", "Everything"};
			const char* layerCurrent = layerTypeItems[layerIndex];
			if (ImGui::BeginCombo("Layer", layerCurrent)) {
				for (int n = 0; n < IM_ARRAYSIZE(layerTypeItems); ++n) {
					if (ImGui::Selectable(layerTypeItems[n])) {
						layerIndex = n;
						if (n == 7) {
							layer = WorldLayers::EVERYTHING;
						} else {
							layer = WorldLayers(1 << layerIndex);
						}
					}
				}
				ImGui::EndCombo();
			}
			ImGui::DragFloat("Radius", &radius, App->editor->dragSpeed2f, 0, inf);
			ImGui::Unindent();
		}
	}

	ImGui::NewLine();
	// Texture Preview
	if (ImGui::CollapsingHeader("Texture Preview")) {
		ResourceTexture* textureResource = App->resources->GetResource<ResourceTexture>(textureID);
		if (textureResource != nullptr) {
			int width;
			int height;
			glGetTextureLevelParameteriv(textureResource->glTexture, 0, GL_TEXTURE_WIDTH, &width);
			glGetTextureLevelParameteriv(textureResource->glTexture, 0, GL_TEXTURE_HEIGHT, &height);

			//if (oldID != textureID) {
			//	ComponentTransform2D* transform2D = GetOwner().GetComponent<ComponentTransform2D>();
			//	if (transform2D != nullptr) {
			//		transform2D->SetSize(float2(static_cast<float>(width), static_cast<float>(height)));
			//	}
			//}

			ImGui::TextWrapped("Size:");
			ImGui::SameLine();
			ImGui::TextWrapped("%i x %i", width, height);
			ImGui::Image((void*) textureResource->glTexture, ImVec2(200, 200));
		}
	}

	ImGui::Unindent();
}

void ComponentParticleSystem::InitParticlePosAndDir(Particle* currentParticle) {
	float x0 = 0, y0 = 0, z0 = 0, x1 = 0, y1 = 0, z1 = 0;
	float3 localPos = float3::zero;
	float3 localDir = float3::zero;

	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();

	if (emitterType == ParticleEmitterType::CONE) {
		float theta = 2 * pi * float(rand()) / float(RAND_MAX);
		if (randomConeRadiusDown) {
			x0 = float(rand()) / float(RAND_MAX) * cos(theta);
			z0 = float(rand()) / float(RAND_MAX) * sin(theta);
		} else {
			x0 = cos(theta);
			z0 = sin(theta);
		}

		if (randomConeRadiusUp) {
			float theta = 2 * pi * float(rand()) / float(RAND_MAX);
			x1 = float(rand()) / float(RAND_MAX) * cos(theta);
			z1 = float(rand()) / float(RAND_MAX) * sin(theta);
		} else {
			x1 = x0;
			z1 = z0;
		}

		float3 localPos0 = float3(x0, 0.0f, z0) * coneRadiusDown;
		float3 localPos1 = float3(x1, 0.0f, z1) * coneRadiusUp + float3::unitY;
		localDir = (localPos1 - localPos0).Normalized();

		if (reverseEffect) {
			localPos = localPos0 + localDir * reverseDistance;
		} else {
			localPos = localPos0;
		}

	} else if (emitterType == ParticleEmitterType::SPHERE) {
		x1 = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;
		y1 = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;
		z1 = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;

		localDir = float3(x1, y1, z1);

		if (reverseEffect) {
			localPos = localDir * reverseDistance;
		} else {
			localPos = float3::zero;
		}
	}

	currentParticle->initialPosition = transform->GetGlobalPosition() + transform->GetGlobalRotation() * localPos;
	currentParticle->position = currentParticle->initialPosition;
	currentParticle->direction = transform->GetGlobalRotation() * localDir;
}

void ComponentParticleSystem::InitParticleScale(Particle* currentParticle) {
	currentParticle->scale = float3(0.1f, 0.1f, 0.1f) * scale;
}

void ComponentParticleSystem::InitParticleVelocity(Particle* currentParticle) {
	currentParticle->velocity = velocity;
}

void ComponentParticleSystem::InitParticleLifetime(Particle* currentParticle) {
	currentParticle->life = life;
}

void ComponentParticleSystem::CreateParticles() {
	particles.Allocate(maxParticles);
}

void ComponentParticleSystem::Load(JsonValue jComponent) {
	// Gizmo
	drawGizmo = jComponent[JSON_TAG_DRAW_GIZMO];

	// Control
	isPlaying = jComponent[JSON_TAG_IS_PLAYING];
	startDelay = jComponent[JSON_TAG_START_DELAY];

	// Particle System
	duration = jComponent[JSON_TAG_DURATION];
	looping = jComponent[JSON_TAG_LOOPING];
	life = jComponent[JSON_TAG_LIFE];
	velocity = jComponent[JSON_TAG_VELOCITY];
	scale = jComponent[JSON_TAG_SCALE];
	reverseEffect = jComponent[JSON_TAG_REVERSE_EFFECT];
	reverseDistance = jComponent[JSON_TAG_REVERSE_DISTANCE];
	maxParticles = jComponent[JSON_TAG_MAX_PARTICLE];

	//Emision
	attachEmitter = jComponent[JSON_TAG_ATTACH_EMITTER];

	// Shape
	emitterType = (ParticleEmitterType)(int) jComponent[JSON_TAG_EMITTER_TYPE];
	coneRadiusUp = jComponent[JSON_TAG_CONE_RADIUS_UP];
	coneRadiusDown = jComponent[JSON_TAG_CONE_RADIUS_DOWN];
	randomConeRadiusUp = jComponent[JSON_TAG_RANDOM_CONE_RADIUS_UP];
	randomConeRadiusDown = jComponent[JSON_TAG_RANDOM_CONE_RADIUS_DOWN];

	// Size over Lifetime
	sizeOverLifetime = jComponent[JSON_TAG_SIZE_OVER_LIFETIME];
	scaleFactor = jComponent[JSON_TAG_SCALE_FACTOR];

	// Color over Lifetime
	colorOverLifetime = jComponent[JSON_TAG_COLOR_OVER_LIFETIME];
	int numberColors = jComponent[JSON_TAG_NUMBER_COLORS];
	if (!gradient) gradient = new ImGradient();
	gradient->clearList();
	JsonValue jColor = jComponent[JSON_TAG_GRADIENT_COLORS];
	for (int i = 0; i < numberColors; ++i) {
		JsonValue jMark = jColor[i];
		gradient->addMark(jMark[4], ImColor((float) jMark[0], (float) jMark[1], (float) jMark[2], (float) jMark[3]));
	}

	// Texture Sheet Animation
	Ytiles = jComponent[JSON_TAG_YTILES];
	Xtiles = jComponent[JSON_TAG_XTILES];
	animationSpeed = jComponent[JSON_TAG_ANIMATION_SPEED];
	isRandomFrame = jComponent[JSON_TAG_IS_RANDOM_FRAME];

	// Render
	textureID = jComponent[JSON_TAG_TEXTURE_TEXTURE_ID];
	if (textureID != 0) {
		App->resources->IncreaseReferenceCount(textureID);
	}
	billboardType = (BillboardType)(int) jComponent[JSON_TAG_BILLBOARD_TYPE];
	renderMode = (ParticleRenderMode)(int) jComponent[JSON_TAG_PARTICLE_RENDER_MODE];

	JsonValue jFlip = jComponent[JSON_TAG_FLIP_TEXTURE];
	flipTexture[0] = jFlip[0];
	flipTexture[1] = jFlip[1];

	// Collision
	collision = jComponent[JSON_TAG_HAS_COLLISION];
	layerIndex = jComponent[JSON_TAG_LAYER_INDEX];
	layer = WorldLayers(1 << layerIndex);

	particleSpawned = 0;
	CreateParticles();
}

void ComponentParticleSystem::Save(JsonValue jComponent) const {
	// Gizmo
	jComponent[JSON_TAG_DRAW_GIZMO] = drawGizmo;

	// Control
	jComponent[JSON_TAG_IS_PLAYING] = isPlaying;
	jComponent[JSON_TAG_START_DELAY] = startDelay;

	// Particle System
	jComponent[JSON_TAG_DURATION] = duration;
	jComponent[JSON_TAG_LOOPING] = looping;
	jComponent[JSON_TAG_LIFE] = life;
	jComponent[JSON_TAG_VELOCITY] = velocity;
	jComponent[JSON_TAG_SCALE] = scale;
	jComponent[JSON_TAG_REVERSE_EFFECT] = reverseEffect;
	jComponent[JSON_TAG_REVERSE_DISTANCE] = reverseDistance;
	jComponent[JSON_TAG_MAX_PARTICLE] = maxParticles;

	//Emision
	jComponent[JSON_TAG_ATTACH_EMITTER] = attachEmitter;

	// Shape
	jComponent[JSON_TAG_EMITTER_TYPE] = (int) emitterType;
	jComponent[JSON_TAG_CONE_RADIUS_UP] = coneRadiusUp;
	jComponent[JSON_TAG_CONE_RADIUS_DOWN] = coneRadiusDown;
	jComponent[JSON_TAG_RANDOM_CONE_RADIUS_UP] = randomConeRadiusUp;
	jComponent[JSON_TAG_RANDOM_CONE_RADIUS_DOWN] = randomConeRadiusDown;

	// Size over Lifetime
	jComponent[JSON_TAG_SIZE_OVER_LIFETIME] = sizeOverLifetime;
	jComponent[JSON_TAG_SCALE_FACTOR] = scaleFactor;

	// Color over Lifetime
	jComponent[JSON_TAG_COLOR_OVER_LIFETIME] = colorOverLifetime;
	int color = 0;
	JsonValue jColor = jComponent[JSON_TAG_GRADIENT_COLORS];
	for (ImGradientMark* mark : gradient->getMarks()) {
		JsonValue jMask = jColor[color];
		jMask[0] = mark->color[0];
		jMask[1] = mark->color[1];
		jMask[2] = mark->color[2];
		jMask[3] = mark->color[3];
		jMask[4] = mark->position;

		color++;
	}
	jComponent[JSON_TAG_NUMBER_COLORS] = gradient->getMarks().size();

	// Texture Sheet Animation
	jComponent[JSON_TAG_YTILES] = Ytiles;
	jComponent[JSON_TAG_XTILES] = Xtiles;
	jComponent[JSON_TAG_ANIMATION_SPEED] = animationSpeed;
	jComponent[JSON_TAG_IS_RANDOM_FRAME] = isRandomFrame;

	// Render
	jComponent[JSON_TAG_TEXTURE_TEXTURE_ID] = textureID;
	jComponent[JSON_TAG_BILLBOARD_TYPE] = (int) billboardType;
	jComponent[JSON_TAG_PARTICLE_RENDER_MODE] = (int) renderMode;
	JsonValue jFlip = jComponent[JSON_TAG_FLIP_TEXTURE];
	jFlip[0] = flipTexture[0];
	jFlip[1] = flipTexture[1];

	// Collision
	jComponent[JSON_TAG_HAS_COLLISION] = collision;
	jComponent[JSON_TAG_LAYER_INDEX] = layerIndex;
}

void ComponentParticleSystem::Update() {
	if (restDelayTime <= 0) {
		if (isPlaying) {
			emitterTime += App->time->GetDeltaTimeOrRealDeltaTime();
		}

		for (Particle& currentParticle : particles) {
			if (executer) {
				currentParticle.life = -1;
			} else {
				UpdatePosition(&currentParticle);

				UpdateLife(&currentParticle);

				if (sizeOverLifetime) {
					UpdateScale(&currentParticle);
				}

				if (!isRandomFrame) {
					currentParticle.currentFrame += animationSpeed * App->time->GetDeltaTimeOrRealDeltaTime();
				}
			}
			if (currentParticle.life < 0) {
				deadParticles.push_back(&currentParticle);
			}
		}
		if (executer) executer = false;
		UndertakerParticle();
		SpawnParticles();

	} else {
		if (!isPlaying) return;
		restDelayTime -= App->time->GetDeltaTimeOrRealDeltaTime();
	}
}

void ComponentParticleSystem::UndertakerParticle() {
	for (Particle* currentParticle : deadParticles) {
		App->physics->RemoveParticleRigidbody(currentParticle);
		RELEASE(currentParticle->motionState);
		particles.Release(currentParticle);
	}
	deadParticles.clear();
}

void ComponentParticleSystem::UpdatePosition(Particle* currentParticle) {
	if (reverseEffect) {
		currentParticle->position -= currentParticle->direction * velocity * App->time->GetDeltaTimeOrRealDeltaTime();
	} else {
		currentParticle->position += currentParticle->direction * velocity * App->time->GetDeltaTimeOrRealDeltaTime();
	}

	// TODO: Implement attachEmmiter properly
	if (attachEmitter) {
		float3 position = GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition();
		if (!currentParticle->emitterPosition.Equals(position)) {
			currentParticle->position = (position - currentParticle->emitterPosition).Normalized() * Length(position - currentParticle->emitterPosition) + currentParticle->position;
			currentParticle->emitterPosition = position;
		}
	}

	if (billboardType == BillboardType::NORMAL) {
		currentParticle->model = float4x4::FromTRS(currentParticle->position, currentParticle->rotation, currentParticle->scale);
	} else {
		currentParticle->modelStretch.SetTranslatePart(currentParticle->position);
	}
}

void ComponentParticleSystem::UpdateScale(Particle* currentParticle) {
	currentParticle->radius *= 1 + scaleFactor * App->time->GetDeltaTimeOrRealDeltaTime() / currentParticle->scale.x;
	if (collision) App->physics->UpdateParticleRigidbody(currentParticle);

	currentParticle->scale.x += scaleFactor * App->time->GetDeltaTimeOrRealDeltaTime();
	currentParticle->scale.y += scaleFactor * App->time->GetDeltaTimeOrRealDeltaTime();
	currentParticle->scale.z += scaleFactor * App->time->GetDeltaTimeOrRealDeltaTime();

	if (currentParticle->scale.x < 0) {
		currentParticle->scale.x = 0;
	}
	if (currentParticle->scale.y < 0) {
		currentParticle->scale.y = 0;
	}
	if (currentParticle->scale.z < 0) {
		currentParticle->scale.z = 0;
	}
}

void ComponentParticleSystem::UpdateLife(Particle* currentParticle) {
	currentParticle->life -= App->time->GetDeltaTimeOrRealDeltaTime();
}

void ComponentParticleSystem::KillParticle(Particle* currentParticle) {
	currentParticle->life = 0;
	App->physics->RemoveParticleRigidbody(currentParticle);
	RELEASE(currentParticle->motionState);
}

void ComponentParticleSystem::SpawnParticles() {
	if ((looping || (particleSpawned < maxParticles && emitterTime < duration)) && isPlaying) {
		SpawnParticleUnit();

	} else {
		if (particles.Count() == 0) {
			restDelayTime = startDelay;
			isPlaying = false;
		}
	}
}

void ComponentParticleSystem::Init() {
	if (!gradient) gradient = new ImGradient();
	CreateParticles();
}

void ComponentParticleSystem::SpawnParticleUnit() {
	Particle* currentParticle = particles.Obtain();
	if (!looping) {
		particleSpawned++;
	}
	if (currentParticle) {
		if (isRandomFrame) {
			currentParticle->currentFrame = static_cast<float>(rand() % ((Xtiles * Ytiles) + 1));
		} else {
			currentParticle->currentFrame = 0;
		}
		InitParticleScale(currentParticle);
		InitParticlePosAndDir(currentParticle);
		InitParticleVelocity(currentParticle);
		InitParticleLifetime(currentParticle);
		currentParticle->emitterPosition = GetOwner().GetComponent<ComponentTransform>()->GetGlobalPosition();

		if (billboardType == BillboardType::STRETCH) {
			float3x3 newRotation = float3x3::FromEulerXYZ(0.f, 0.f, pi / 2);
			currentParticle->modelStretch = currentParticle->model * newRotation;
		}

		if (collision) {
			currentParticle->emitter = this;
			currentParticle->radius = radius;
			App->physics->CreateParticleRigidbody(currentParticle);
		}
	}
}

void ComponentParticleSystem::DestroyParticlesColliders() {
	for (Particle& currentParticle : particles) {
		App->physics->RemoveParticleRigidbody(&currentParticle);
		RELEASE(currentParticle.motionState);
	}
}

void ComponentParticleSystem::DrawGizmos() {
	if (IsActive() && drawGizmo) {
		if (emitterType == ParticleEmitterType::CONE) {
			ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
			dd::cone(transform->GetGlobalPosition(), transform->GetGlobalRotation() * float3::unitY * 1, dd::colors::White, coneRadiusUp, coneRadiusDown);
		}
		if (emitterType == ParticleEmitterType::SPHERE) {
			ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
			dd::sphere(transform->GetGlobalPosition(), dd::colors::White, 1.0f);
		}
	}
}

void ComponentParticleSystem::Draw() {
	if (isPlaying) {
		for (Particle& currentParticle : particles) {
			ProgramBillboard* program = App->programs->billboard;
			glUseProgram(program->program);

			unsigned glTexture = 0;
			ResourceTexture* texture = App->resources->GetResource<ResourceTexture>(textureID);
			glTexture = texture ? texture->glTexture : 0;
			int hasDiffuseMap = texture ? 1 : 0;

			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			if (renderMode == ParticleRenderMode::ADDITIVE) {
				glBlendFunc(GL_ONE, GL_ONE);
			} else {
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}

			glBindBuffer(GL_ARRAY_BUFFER, App->userInterface->GetQuadVBO());
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(float) * 6 * 3));

			ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
			float3x3 rotatePart = currentParticle.model.RotatePart();
			float3x3 transformRotatePart = transform->GetGlobalMatrix().RotatePart();
			Frustum* frustum = App->camera->GetActiveCamera()->GetFrustum();

			float4x4 modelMatrix;
			if (billboardType == BillboardType::NORMAL) {
				float4x4 newModelMatrix = currentParticle.model.LookAt(rotatePart.Col(2), -frustum->Front(), rotatePart.Col(1), float3::unitY);
				modelMatrix = float4x4::FromTRS(currentParticle.position, newModelMatrix.RotatePart(), currentParticle.scale);
			} else if (billboardType == BillboardType::STRETCH) {
				float3 cameraPos = App->camera->GetActiveCamera()->GetFrustum()->Pos();
				float3 cameraDir = (cameraPos - currentParticle.position).Normalized();
				float3 upDir = Cross(currentParticle.direction, cameraDir);
				float3 newCameraDir = Cross(currentParticle.direction, upDir);

				float3x3 newRotation;
				newRotation.SetCol(0, upDir);
				newRotation.SetCol(1, currentParticle.direction);
				newRotation.SetCol(2, newCameraDir);

				modelMatrix = float4x4::FromTRS(currentParticle.position, newRotation * currentParticle.modelStretch.RotatePart(), currentParticle.scale);
			} else if (billboardType == BillboardType::HORIZONTAL) {
				float4x4 newModelMatrix = currentParticle.model.LookAt(rotatePart.Col(2), float3::unitY, rotatePart.Col(1), float3::unitY);
				modelMatrix = float4x4::FromTRS(currentParticle.position, newModelMatrix.RotatePart(), currentParticle.scale);
			} else if (billboardType == BillboardType::VERTICAL) {
				float3 cameraPos = App->camera->GetActiveCamera()->GetFrustum()->Pos();
				float3 cameraDir = (float3(cameraPos.x, currentParticle.position.y, cameraPos.z) - currentParticle.position).Normalized();
				float4x4 newModelMatrix = currentParticle.model.LookAt(rotatePart.Col(2), cameraDir, rotatePart.Col(1), float3::unitY);
				modelMatrix = float4x4::FromTRS(currentParticle.position, newModelMatrix.RotatePart(), currentParticle.scale);
			}

			float4x4* proj = &App->camera->GetProjectionMatrix();
			float4x4* view = &App->camera->GetViewMatrix();

			glUniformMatrix4fv(program->modelLocation, 1, GL_TRUE, modelMatrix.ptr());
			glUniformMatrix4fv(program->viewLocation, 1, GL_TRUE, view->ptr());
			glUniformMatrix4fv(program->projLocation, 1, GL_TRUE, proj->ptr());

			float4 color = float4::one;
			if (colorOverLifetime) {
				float factor = 1 - currentParticle.life / life; // Life decreases from Life to 0
				gradient->getColorAt(factor, color.ptr());
			}

			glUniform1i(program->diffuseMapLocation, 0);
			glUniform1i(program->hasDiffuseLocation, hasDiffuseMap);
			glUniform4fv(program->inputColorLocation, 1, color.ptr());

			glUniform1f(program->currentFrameLocation, currentParticle.currentFrame);

			glUniform1i(program->xTilesLocation, Xtiles);
			glUniform1i(program->yTilesLocation, Ytiles);

			glUniform1i(program->xFlipLocation, flipTexture[0]);
			glUniform1i(program->yFlipLocation, flipTexture[1]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, glTexture);

			//TODO: implement drawarrays
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);

			if (App->renderer->drawColliders) {
				dd::sphere(currentParticle.position, dd::colors::LawnGreen, currentParticle.radius);
			}
		}
	}
}

void ComponentParticleSystem::Play() {
	if (!isPlaying) {
		isPlaying = true;
		particleSpawned = 0;
		restDelayTime = startDelay;
		emitterTime = 0.0f;
	}
}

void ComponentParticleSystem::Stop() {
	particleSpawned = maxParticles;
	executer = true;
}