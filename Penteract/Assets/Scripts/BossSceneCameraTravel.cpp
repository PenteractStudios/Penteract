#include "BossSceneCameraTravel.h"

EXPOSE_MEMBERS(BossSceneCameraTravel) {
	MEMBER(MemberType::FLOAT, travellingAcceleration),
	MEMBER(MemberType::FLOAT, travellingDeceleration),
	MEMBER(MemberType::FLOAT, maxTravellingSpeed),
	MEMBER(MemberType::FLOAT, startingTravellingSpeed),
	MEMBER(MemberType::FLOAT, decelerationDistance),
	MEMBER(MemberType::FLOAT, finishDistanceThreshold)
};

GENERATE_BODY_IMPL(BossSceneCameraTravel);

void BossSceneCameraTravel::Start() {

}

void BossSceneCameraTravel::Update() {

}