#include "GodModeUI.h"

#include "GameplaySystems.h"
#include "GameObject.h"

#include "Components/UI/ComponentToggle.h"

EXPOSE_MEMBERS(GodModeUI) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
};

GENERATE_BODY_IMPL(GodModeUI);

void GodModeUI::Start() {
    gameObject = &GetOwner();
	for (GameObject* child : gameObject->GetChildren()) {
		if (child->HasComponent<ComponentToggle>()) {
            ComponentToggle* toggle = child->GetComponent<ComponentToggle>();
            if (toggle) {
                // Debug::Log(toggle->IsChecked() ? "is checked" : "isn't checked");
			    children.push_back(child);
            }            
		}
	}
	
}

void GodModeUI::Update() {
	
    
}

/* 
on child toggle(index)
switch index
call respective function
 */

/* 
onToggled 
getOwner-GetParent()->GET_SCRIPT->onchildtoggle(1)
*/