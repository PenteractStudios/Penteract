#include "UISpriteSheetPlayer.h"
#include "GameObject.h"
#include "GameplaySystems.h"
EXPOSE_MEMBERS(UISpriteSheetPlayer) {
	MEMBER(MemberType::FLOAT, secondsPerFrame),
		MEMBER(MemberType::BOOL, loops),
		MEMBER(MemberType::BOOL, playOnAwake)
};

GENERATE_BODY_IMPL(UISpriteSheetPlayer);

void UISpriteSheetPlayer::Start() {
	frames = GetOwner().GetChildren();
	if (frames.size() > 0) {
		currentFrameObj = frames[0];
		for (int i = 0; i < frames.size(); i++) {
			frames[i]->Disable();
		}
	}
	if (playOnAwake) {
		Play();
	}
}

void UISpriteSheetPlayer::Update() {
	if (frames.size() == 0) return;
	if (!playing)return;


	if (animationTimer >= secondsPerFrame) {
		currentFrame++;
		animationTimer = 0.0f;

		if (currentFrameObj)
			currentFrameObj->Disable();

		if (currentFrame >= frames.size()) {
			if (!loops) {
				playing = false;
			}
			currentFrame = 0;
			return;
		}

		currentFrameObj = frames[currentFrame];

		if (currentFrameObj)
			currentFrameObj->Enable();
	}

	animationTimer += Time::GetDeltaTime();

}

void UISpriteSheetPlayer::Play() {
	playing = true;
}

void UISpriteSheetPlayer::Stop() {
	playing = false;
}

