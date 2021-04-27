#pragma once

#include "Panel.h"

#include "Math/float2.h"

class PanelScene : public Panel {
public:
	PanelScene();

	void Update() override;

	bool IsUsing2D() const;

	// Getters
	const float2& GetMousePosOnScene() const;
	const char* GetCurrentShadingMode() const;

private:
	float2 framebufferSize = {0.0f, 0.0f};
	float2 mousePosOnScene = {0.0f, 0.0f};

	const char* currentShadingMode = "Shaded";

	bool view2D = false;
};
