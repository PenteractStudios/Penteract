#include "CursorAim.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(CursorAim) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
    MEMBER(MemberType::GAME_OBJECT_UID, cameraUID),
    MEMBER(MemberType::GAME_OBJECT_UID, fangUID),
};

GENERATE_BODY_IMPL(CursorAim);

void CursorAim::Start() {
    GameObject* cameraAux = GameplaySystems::GetGameObject(cameraUID);
    if (cameraAux) {
        lookAtMouseCameraComp = cameraAux->GetComponent<ComponentTransform>();
    }

    fang = GameplaySystems::GetGameObject(fangUID);
    if (fang) {
        playerController = GET_SCRIPT(fang, PlayerController);
    }
}

void CursorAim::Update() {

    if (playerController->playerFang.isAiming()) {
        float2 mousePosition = Input::GetMousePositionNormalized();
        Debug::Log(std::to_string(mousePosition.x).c_str());

        if (mousePosition.x > 0.75) {
            Quat newRotation = lookAtMouseCameraComp->GetGlobalRotation();
            Quat aux = newRotation.RotateY(0.261799);
            newRotation.y = aux.y;
            lookAtMouseCameraComp->SetRotation(newRotation);
        }
        else {
            Quat newRotation = lookAtMouseCameraComp->GetGlobalRotation();
            Quat aux = newRotation.RotateY(1.72788);
            newRotation.y = aux.y;
            lookAtMouseCameraComp->SetRotation(newRotation);
        }
       // if (GetControllerOrientationDirection().x > 1) {
       //}
    }
    else {
        Quat newRotation = lookAtMouseCameraComp->GetGlobalRotation();
        Quat aux = newRotation.RotateY(1.72788);
        newRotation.y = aux.y;
        lookAtMouseCameraComp->SetRotation(newRotation);
    }
}




