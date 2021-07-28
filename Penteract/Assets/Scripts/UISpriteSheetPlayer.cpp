#include "UISpriteSheetPlayer.h"
#include "GameObject.h"
#include "GameplaySystems.h"
EXPOSE_MEMBERS(UISpriteSheetPlayer) {
	MEMBER(MemberType::FLOAT, secondsPerFrame)
};

GENERATE_BODY_IMPL(UISpriteSheetPlayer);

void UISpriteSheetPlayer::Start() {
	frames = GetOwner().GetChildren();
	if (frames.size() > 0) {
		currentFrameObj = frames[0];
		for (int i = 1; i < frames.size(); i++) {
			frames[i]->Disable();
		}
	}
}

void UISpriteSheetPlayer::Update() {
	if (frames.size() == 0) return;

	if (animationTimer >= secondsPerFrame) {
		currentFrame++;
		animationTimer = 0.0f;

		if (currentFrame >= frames.size()) {
			currentFrame = 0;
		}
		if (currentFrameObj)
			currentFrameObj->Disable();

		currentFrameObj = frames[currentFrame];

		if (currentFrameObj)
			currentFrameObj->Enable();
	}

	animationTimer += Time::GetDeltaTime();



}