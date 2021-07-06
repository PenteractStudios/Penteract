#include "VSyncSetter.h"

#include "GameplaySystems.h"
#include "Components/UI/ComponentToggle.h"

EXPOSE_MEMBERS(VSyncSetter) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(VSyncSetter);

void VSyncSetter::Start() {
	ComponentToggle* toggle = GetOwner().GetComponent<ComponentToggle>();
	if (toggle) {
		toggle->SetChecked(Screen::IsVSyncActive());
	}
}

void VSyncSetter::Update() {
	
}

void VSyncSetter::OnToggled(bool toggled) {
	Screen::SetVSync(toggled);
}
