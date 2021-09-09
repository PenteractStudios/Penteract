#include "AdvertScrip.h"
#include "GameplaySystems.h"


EXPOSE_MEMBERS(AdvertScrip) {
        MEMBER(MemberType::FLOAT, speed),
        MEMBER(MemberType::FLOAT, startTime),
        MEMBER(MemberType::FLOAT, restTime),
};

GENERATE_BODY_IMPL(AdvertScrip);

void AdvertScrip::Start() {
	
}

void AdvertScrip::Update() {
    //Go Down
    if (startTime >= 0) {
    }
    if (offset.x < 0.5) {
        offset += float2(0, speed) * Time::GetDeltaTime();
        GetOwner().GetComponent<ComponentMeshRenderer>()->SetTextureOffset(offset);
    }
    else {

    }
}