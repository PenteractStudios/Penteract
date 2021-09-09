#include "AdvertScrip.h"
#include "GameplaySystems.h"


EXPOSE_MEMBERS(AdvertScrip) {
        MEMBER(MemberType::FLOAT, speed),
        MEMBER(MemberType::FLOAT, startTime),
        MEMBER(MemberType::FLOAT, changeTime),
};

GENERATE_BODY_IMPL(AdvertScrip);

void AdvertScrip::Start() {
    restTime = changeTime;
}

void AdvertScrip::Update() {
    //Go Down
    if (startTime > 0) {
        startTime -= Time::GetDeltaTime();
    }
    else {
        if (isChanging) {
            if (firstAdvert) {
                if (offset.y < 0.5) {
                    offset += float2(0, speed) * Time::GetDeltaTime();
                    GetOwner().GetComponent<ComponentMeshRenderer>()->SetTextureOffset(offset);
                }
                else {
                    offset.y = 0.5;
                    isChanging = false;
                }
            }
            if (!firstAdvert) {
                if ((offset.y > 0)) {
                    offset -= float2(0, speed) * Time::GetDeltaTime();
                    GetOwner().GetComponent<ComponentMeshRenderer>()->SetTextureOffset(offset);
                }
                else {
                    offset.y = 0.0;
                    isChanging = false;
                }
            }
        }
        else {
            if (restTime > 0) {
                restTime -= Time::GetDeltaTime();
            }
            else {
                isChanging = true;
                restTime = changeTime;
                if (firstAdvert) { 
                    firstAdvert = false; 
                }
                else
                {
                    firstAdvert = true;
                }
            }
        }
    }
}