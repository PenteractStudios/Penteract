#include "OnimaruMinigun.h"
#include "GameplaySystems.h"
#include "CameraController.h"
#include "GameController.h"
#include "Geometry/Plane.h"
#include "Geometry/Frustum.h"
#include "Geometry/LineSegment.h"
#include "Player.h"


EXPOSE_MEMBERS(OnimaruMinigun) {
    // Add members here to expose them to the engine. Example:
    // MEMBER(MemberType::BOOL, exampleMember1),
    // MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
    // MEMBER(MemberType::GAME_OBJECT_UID, exampleMember3)
	MEMBER(MemberType::GAME_OBJECT_UID, player)
};

GENERATE_BODY_IMPL(OnimaruMinigun);

void OnimaruMinigun::Start() {
	//if (player) {
	//	playerScript = GET_SCRIPT(player, Player);
	//}
}

void OnimaruMinigun::Update() {

}

