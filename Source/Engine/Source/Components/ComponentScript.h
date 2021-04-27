#pragma once

#include "Component.h"

class ComponentScript : public Component {
public:
	REGISTER_COMPONENT(ComponentScript, ComponentType::SCRIPT, true);

	void Update() override;
	void OnStart();
	void OnEditorUpdate() override;
	void Save(JsonValue jComponent) const override;
	void Load(JsonValue jComponent) override;
	void DuplicateComponent(GameObject& owner) override;

	UID GetScriptID() const;

private:

	void ReloadScript();

public:
	bool dirty = false;

private:
	UID scriptID = 0;
};