#pragma once

#include "Components/Component.h"
#include "Components/UI/ComponentTransform2D.h"

#include "Math/float3.h"
#include <array>
#include <vector>

// Component that renders a Text
class ComponentText : public Component {
public:
	REGISTER_COMPONENT(ComponentText, ComponentType::TEXT, false);

	~ComponentText();

	void Init() override;			// Generates VBO and VAO of ComponentText
	void OnEditorUpdate() override; // Works as input of Text, FontSize, Color and ShaderID and FontID

	void Save(JsonValue jComponent) const override;			// Serializes
	void Load(JsonValue jComponent) override;				// Deserializes
	void DuplicateComponent(GameObject& owner) override;	// Duplicates Component
	
	void Draw(ComponentTransform2D* transform) const;				// Draws the text ortographically using the active camera and the position of the Tranform2D. It will apply the color as tint
	TESSERACT_ENGINE_API void SetText(const std::string& newText);	// Sets text
	TESSERACT_ENGINE_API void SetFontSize(float newfontSize);		// Sets fontSize
	TESSERACT_ENGINE_API void SetFontColor(const float4& newColor); // Sets color
	float4 GetFontColor() const;									// Returns Color
	void RecalculcateVertices();									// Recalculate verticesText. This is called when Text/Font/Transform is modified in order to recalculate the position of vertices.

private:
	std::string text = "Text";									   // Text to display
	std::vector<std::array<std::array<float, 4>, 6>> verticesText; // Vertices per each character

	float fontSize = 12.0f;		// Font size
	float4 color = float4::one; // Color of the font

	unsigned int vbo = 0; // VBO of the text
	unsigned int vao = 0; // VAO of the text
	UID shaderID = 0;	  // Shader ID of the text
	UID fontID = 0;		  // Font ID of the text
};