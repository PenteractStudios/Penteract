#include "ComponentTrail.h"

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
#include "Panels/PanelScene.h"
#include "Resources/ResourceTexture.h"
#include "Resources/ResourceShader.h"
#include "FileSystem/TextureImporter.h"
#include "FileSystem/JsonValue.h"
#include "Math/float3x3.h"
#include "Utils/ImGuiUtils.h"
#include "Math/TransformOps.h"
#include "imgui.h"
#include "GL/glew.h"
#include "debugdraw.h"

#include <Utils/Logging.h>

#include "Utils/Leaks.h"

#define JSON_TAG_TEXTURE_TEXTUREID "TextureId"
#define JSON_TAG_TIMETOSTART "TimeToStart"
#define JSON_TAG_MAXVERTICES "MaxVertices"
#define JSON_TAG_TRAILQUADS "TrailQuads"
#define JSON_TAG_ALPHATRANSPARENCY "AlphaTransparency"

// clang-format off
static const float textureCords[12] = {
	// Front (x, y, z)
	0.0f,0.0f,
	1.0f,0.0f,
	0.0f,1.0f,
	//////////
	1.0f,0.0f,
	1.0f,1.0f,
	0.0f, 1.0f,
	};
// clang-format on

void ComponentTrail::Init() {
	glGenBuffers(1, &quadVBO);
}

void ComponentTrail::Update() {
	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();
	if (isStarted) {
		previousPositionUp = currentPositionUp;
		previousPositionDown = currentPositionDown;
		previousPosition = currentPosition;

		currentPosition = transform->GetGlobalPosition();
		previousVectorUp = transform->GetGlobalRotation() * float3::unitY;
		previousVectorUp.Normalize();

		currentPositionUp = (previousVectorUp * width) + currentPosition;
		currentPositionDown = (-previousVectorUp * width) + currentPosition;
		if (trianglesCreated >= (maxVertices)) {
			UpdateVerticesPosition();
			trianglesCreated -= 30;
		}

		InsertVertex(previousPositionDown);
		InsertTextureCoords();
		InsertVertex(currentPositionDown);
		InsertTextureCoords();
		InsertVertex(previousPositionUp);
		InsertTextureCoords();

		InsertVertex(currentPositionDown);
		InsertTextureCoords();
		InsertVertex(currentPositionUp);
		InsertTextureCoords();
		InsertVertex(previousPositionUp);
		InsertTextureCoords();

		quadsCreated++;
	} else {
		isStarted = true;
		currentPosition = transform->GetGlobalPosition();
		previousVectorUp = transform->GetGlobalRotation() * float3::unitY;
		previousVectorUp.Normalize();
		currentPositionUp = previousVectorUp * width + currentPosition;
		currentPositionDown = -previousVectorUp * width + currentPosition;
	}
}

void ComponentTrail::OnEditorUpdate() {
	ImGui::DragFloat("Witdh", &width, App->editor->dragSpeed2f, 0, inf);
	if (ImGui::DragScalar("Trail Quads", ImGuiDataType_U32, &trailQuads)) {
		if (trailQuads <= 0) trailQuads = 1;
		if (trailQuads > 50) trailQuads = 50;
		DeleteQuads();
	}

	UID oldID = textureID;
	ImGui::ResourceSlot<ResourceTexture>("texture", &textureID);
	ResourceTexture* textureResource = App->resources->GetResource<ResourceTexture>(textureID);
	if (textureResource != nullptr) {
		int width;
		int height;
		glGetTextureLevelParameteriv(textureResource->glTexture, 0, GL_TEXTURE_WIDTH, &width);
		glGetTextureLevelParameteriv(textureResource->glTexture, 0, GL_TEXTURE_HEIGHT, &height);

		if (oldID != textureID) {
			ComponentTransform2D* transform2D = GetOwner().GetComponent<ComponentTransform2D>();
			if (transform2D != nullptr) {
				transform2D->SetSize(float2((float) width, (float) height));
			}
		}
		ImGui::NewLine();
		ImGui::Separator();
		ImGui::TextColored(App->editor->titleColor, "Texture Preview");
		ImGui::TextWrapped("Size:");
		ImGui::SameLine();
		ImGui::TextWrapped("%d x %d", width, height);
		ImGui::Image((void*) textureResource->glTexture, ImVec2(200, 200));
		ImGui::Separator();
	}
}

void ComponentTrail::Load(JsonValue jComponent) {
	textureID = jComponent[JSON_TAG_TEXTURE_TEXTUREID];
	if (textureID != 0) {
		App->resources->IncreaseReferenceCount(textureID);
	}
	maxVertices = jComponent[JSON_TAG_MAXVERTICES];
	trailQuads = jComponent[JSON_TAG_TRAILQUADS];
}

void ComponentTrail::Save(JsonValue jComponent) const {
	jComponent[JSON_TAG_TEXTURE_TEXTUREID] = textureID;
	jComponent[JSON_TAG_MAXVERTICES] = maxVertices;
	jComponent[JSON_TAG_TRAILQUADS] = trailQuads;
}

void ComponentTrail::Draw() {
	unsigned int program = App->programs->trail;

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glDisable(GL_CULL_FACE);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO); // set vbo active
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPosition), verticesPosition, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) (sizeof(float) * 3));
	glUseProgram(program);

	ComponentTransform* transform = GetOwner().GetComponent<ComponentTransform>();

	Frustum* frustum = App->camera->GetActiveCamera()->GetFrustum();
	float4x4* proj = &App->camera->GetProjectionMatrix();
	float4x4* view = &App->camera->GetViewMatrix();

	glActiveTexture(GL_TEXTURE0);

	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view->ptr());
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, proj->ptr());

	ResourceTexture* textureResource = App->resources->GetResource<ResourceTexture>(textureID);
	if (textureResource != nullptr) {
		glBindTexture(GL_TEXTURE_2D, textureResource->glTexture);
	}

	glDrawArrays(GL_TRIANGLES, 0, trianglesCreated);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void ComponentTrail::UpdateVerticesPosition() {
	for (int i = 0; i < (maxVertices - 30); i++) {
		verticesPosition[i] = verticesPosition[i + 30];
	}
}
void ComponentTrail::InsertVertex(float3 vertex) {
	verticesPosition[trianglesCreated++] = vertex.x;
	verticesPosition[trianglesCreated++] = vertex.y;
	verticesPosition[trianglesCreated++] = vertex.z;
}

void ComponentTrail::InsertTextureCoords() {
	if (textureCreated == 12) textureCreated = 0;

	verticesPosition[trianglesCreated++] = textureCords[textureCreated++];
	verticesPosition[trianglesCreated++] = textureCords[textureCreated++];
}

void ComponentTrail::DeleteQuads() {
	for (int i = 0; i < (maxVertices); i++) {
		verticesPosition[i] = 0.0f;
	}
	isStarted = false;
	quadsCreated = 0;
	trianglesCreated = 0;
	maxVertices = 30 * trailQuads;
	textureCreated = 0;
}