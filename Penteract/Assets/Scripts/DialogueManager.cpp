#include "DialogueManager.h"

#include "GameplaySystems.h"
#include "GameController.h"
#include "PlayerController.h"
#include "CameraController.h"
#include "GameObject.h"
#include "Components/UI/ComponentText.h"

EXPOSE_MEMBERS(DialogueManager) {
	MEMBER(MemberType::GAME_OBJECT_UID, playerUID),
	MEMBER(MemberType::GAME_OBJECT_UID, gameCameraUID),
	MEMBER_SEPARATOR("Dialogue (sub)Text UIDs"),
	MEMBER(MemberType::GAME_OBJECT_UID, fangTextObjectUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruTextObjectUID),
	MEMBER(MemberType::GAME_OBJECT_UID, dukeTextObjectUID),
	MEMBER(MemberType::GAME_OBJECT_UID, doorTextObjectUID),
	MEMBER_SEPARATOR("Tutorial Objects UIDs"),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialFangUID),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialOnimaruUID),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialSwapUID),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialUpgrades1UID),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialUpgrades2UID),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialUpgrades3UID),
	MEMBER_SEPARATOR("Fade In/Out configuration"),
	MEMBER(MemberType::FLOAT3, dialogueStartPosition),
	MEMBER(MemberType::FLOAT3, dialogueEndPosition),
	MEMBER(MemberType::FLOAT3, tutorialStartPosition),
	MEMBER(MemberType::FLOAT3, tutorialEndPosition),
	MEMBER(MemberType::FLOAT, appearAnimationTime),
	MEMBER(MemberType::FLOAT, disappearAnimationTime)
};

GENERATE_BODY_IMPL(DialogueManager);

void DialogueManager::Start() {
	// Get player controller
	player = GameplaySystems::GetGameObject(playerUID);
	if (player) playerControllerScript = GET_SCRIPT(player, PlayerController);

	// Get camera controller
	camera = GameplaySystems::GetGameObject(gameCameraUID);
	if (camera) cameraControllerScript = GET_SCRIPT(camera, CameraController);

	// Get dialogue windows
	GameObject* fangTextObject = GameplaySystems::GetGameObject(fangTextObjectUID);
	GameObject* onimaruTextObject = GameplaySystems::GetGameObject(onimaruTextObjectUID);
	GameObject* dukeTextObject = GameplaySystems::GetGameObject(dukeTextObjectUID);
	GameObject* doorTextObject = GameplaySystems::GetGameObject(doorTextObjectUID);
	if (fangTextObject && onimaruTextObject && dukeTextObject && doorTextObject) {
		fangTextComponent = fangTextObject->GetComponent<ComponentText>();
		onimaruTextComponent = onimaruTextObject->GetComponent<ComponentText>();
		dukeTextComponent = dukeTextObject->GetComponent<ComponentText>();
		doorTextComponent = doorTextObject->GetComponent<ComponentText>();
	}

	// Get tutorials
	tutorialFang = GameplaySystems::GetGameObject(tutorialFangUID);
	tutorialOnimaru = GameplaySystems::GetGameObject(tutorialOnimaruUID);
	tutorialSwap = GameplaySystems::GetGameObject(tutorialSwapUID);
	tutorialUpgrades1 = GameplaySystems::GetGameObject(tutorialUpgrades1UID);
	tutorialUpgrades2 = GameplaySystems::GetGameObject(tutorialUpgrades2UID);
	tutorialUpgrades3 = GameplaySystems::GetGameObject(tutorialUpgrades3UID);

	// ----- DIALOGUES INIT -----
	// UPGRADES
	dialoguesArray[0] = Dialogue(DialogueWindow::UPGRADES1, true, "", &dialoguesArray[1]);
	dialoguesArray[1] = Dialogue(DialogueWindow::ONIMARU, true, "Hey Fang, look at this.", &dialoguesArray[2]);
	dialoguesArray[2] = Dialogue(DialogueWindow::FANG, true, "Hmm...\nIt looks like Milibot has been\nresearching in some\nnew technologies...", &dialoguesArray[3]);
	dialoguesArray[3] = Dialogue(DialogueWindow::FANG, true, "I might be able to\nseize its power for ourselves\nif we find a couple more.", nullptr);

	dialoguesArray[4] = Dialogue(DialogueWindow::UPGRADES2, true, "", nullptr);

	dialoguesArray[5] = Dialogue(DialogueWindow::UPGRADES3, true, "", &dialoguesArray[6]);
	dialoguesArray[6] = Dialogue(DialogueWindow::FANG, true, "I think I got it...\nYou can power it up this way,\nthen plug it in our core...", &dialoguesArray[7]);
	dialoguesArray[7] = Dialogue(DialogueWindow::FANG, true, "WHOAH!\nOni, check this out!", &dialoguesArray[8]);
	dialoguesArray[8] = Dialogue(DialogueWindow::ONIMARU, true, "I am not sure about this Fang...\nBut OK.\nI trust you.", nullptr);

	// LEVEL 1 - START
	dialoguesArray[9] = Dialogue(DialogueWindow::DUKE, true, "Who do you think you are\nyou son of a...", &dialoguesArray[10]);
	dialoguesArray[10] = Dialogue(DialogueWindow::DUKE, true, "...Fang. It’s been a while!\nWhat do you think about\nthis plating, impressive huh?", &dialoguesArray[11]);
	dialoguesArray[11] = Dialogue(DialogueWindow::DUKE, true, "You would be unstoppable\nif you hadn’t left.\nSo you decided to come back?", &dialoguesArray[12]);
	dialoguesArray[12] = Dialogue(DialogueWindow::FANG, true, "You wish. I'm here to kill you.\n50 million and the pleasure\nof doing it myself.", &dialoguesArray[13]);
	dialoguesArray[13] = Dialogue(DialogueWindow::DUKE, true, "I made you what you are,\neven if you hate it!\nPart of you is mine,\nand you should be grateful.", &dialoguesArray[14]);
	dialoguesArray[14] = Dialogue(DialogueWindow::DUKE, true, "But well... Let’s see how an outdated\npile of junk like you performs\nagainst my latest designs...\nSecurity!", nullptr);

	// FANG TUTORIAL
	dialoguesArray[15] = Dialogue(DialogueWindow::ONIMARU, true, "He is running away!\nDon’t let him escape!", &dialoguesArray[16]);
	dialoguesArray[16] = Dialogue(DialogueWindow::TUTO_FANG, true, "", nullptr);
	// TODO: Fang Ultimate dialogue?

	// SWAP DIALOGUE + ONIMARU TUTORIAL
	dialoguesArray[18] = Dialogue(DialogueWindow::FANG, true, "Onimaru,\nget the repair bots ready...\nI’m gonna need a break.", &dialoguesArray[19]);
	dialoguesArray[19] = Dialogue(DialogueWindow::ONIMARU, true, "Roger.\nInitialising Matter-Switch.", &dialoguesArray[20]);
	dialoguesArray[20] = Dialogue(DialogueWindow::TUTO_SWAP, true, "", &dialoguesArray[21], InputActions::SWITCH);
	dialoguesArray[21] = Dialogue(DialogueWindow::ONIMARU, true, "Long hallways\nis where I perform best.\nWatch how it is done.", &dialoguesArray[22]);
	dialoguesArray[22] = Dialogue(DialogueWindow::TUTO_ONIMARU, true, "", nullptr);
	// TODO: Onimaru Ultimate dialogue?

	// LEVEL 1 - PRE-TRANSPORT
	//dialoguesArray[24] = Dialogue(DialogueWindow::ONIMARU, false, "Are you formulating any plan\nto go through Duke’s plating?\nThat is a strong one, to have protected him\nfrom that headshot.", &dialoguesArray[25]);
	//dialoguesArray[25] = Dialogue(DialogueWindow::FANG, false, "I’m formulating a plan\nto not get blown up by\nthese explosive barrels.", nullptr);

	// LEVEL 1 - PRE-SECURITY
	//dialoguesArray[26] = Dialogue(DialogueWindow::FANG, false, "I wonder why Duke is running away.\nHe seems strong enough to face us by himself...", &dialoguesArray[27]);
	//dialoguesArray[27] = Dialogue(DialogueWindow::ONIMARU, false, "He is going to the Milibot factory.\nWho knows what weapons he is hiding in there...\nOr maybe he is just testing us again.", nullptr);

	// LEVEL 1 - FINAL
	dialoguesArray[28] = Dialogue(DialogueWindow::DOOR, true, "Oh hello guys!\nNice fights there, right?\nWhere have you been?\nI haven’t seen you in a while!", &dialoguesArray[29]);
	dialoguesArray[29] = Dialogue(DialogueWindow::ONIMARU, true, "It has been five years.\nOpen up. We need to come in.", &dialoguesArray[30]);
	dialoguesArray[30] = Dialogue(DialogueWindow::DOOR, true, "Aw… I'm sorry.\nSeems like Duke revoked your access.\nI don’t understand why though...", &dialoguesArray[31]);
	dialoguesArray[31] = Dialogue(DialogueWindow::FANG, true, "Come on...\nWe are friends right?\nJust pretend you malfunctioned\nor something, we won’t tell.", &dialoguesArray[32]);
	dialoguesArray[32] = Dialogue(DialogueWindow::DOOR, true, "Fine... but it's on you.\nI’m taking no responsibility.", &dialoguesArray[33]);
	dialoguesArray[33] = Dialogue(DialogueWindow::ONIMARU, true, "Thanks, Door.", &dialoguesArray[34]);
	dialoguesArray[34] = Dialogue(DialogueWindow::DOOR, true, "Take care in there!\nDon’t get hurt!", nullptr);
}

void DialogueManager::Update() {
	if (!fangTextComponent || !onimaruTextComponent) return;
	if (!tutorialFang || !tutorialOnimaru || !tutorialSwap) return;
	if (!tutorialUpgrades1 || !tutorialUpgrades2 || !tutorialUpgrades3) return;
	if (!player || !camera) return;

	if (activeDialogue) {
		if (runOpenAnimation) ActivateDialogue(activeDialogue);

		if (Player::GetInputBool(activeDialogue->closeButton, PlayerController::useGamepad) && !(runOpenAnimation || runChangeAnimation || runCloseAnimation) && activeDialogueObject) {
			if (activeDialogue->nextDialogue) {
				runChangeAnimation = true;
				if ((static_cast<int>(activeDialogue->character) < 5 && static_cast<int>(activeDialogue->nextDialogue->character) >= 5) ||
					(static_cast<int>(activeDialogue->character) >= 5 && static_cast<int>(activeDialogue->nextDialogue->character) < 5)) {
					runCloseAnimation = true;
					runSecondaryOpen = true;
				}
			} else runCloseAnimation = true;
		}

		if (runChangeAnimation && !runCloseAnimation) {
			ActivateNextDialogue(activeDialogue);
		}

		if (runCloseAnimation) CloseDialogue(activeDialogue);
	}
}

void DialogueManager::SetActiveDialogue(Dialogue* dialogue, bool runAnimation) {
	// Disable last active dialogue
	if (activeDialogueObject) {
		if (activeDialogueObject->IsActive()) {
			activeDialogueObject->Disable();
		}
	}

	if (dialogue) {
		// Set the transition positions that correspond to the new active dialogue
		if (static_cast<int>(dialogue->character) >= 5) {
			currentStartPosition = tutorialStartPosition;
			currentEndPosition = tutorialEndPosition;
		} else {
			currentStartPosition = dialogueStartPosition;
			currentEndPosition = dialogueEndPosition;
		}

		// Activation steps, corresponding to the dialogue type
		activeDialogue = dialogue;
		switch (dialogue->character) {
		case DialogueWindow::FANG:
			activeDialogueObject = fangTextComponent->GetOwner().GetParent();
			fangTextComponent->SetText(dialogue->text);
			break;
		case DialogueWindow::ONIMARU:
			activeDialogueObject = onimaruTextComponent->GetOwner().GetParent();
			onimaruTextComponent->SetText(dialogue->text);
			break;
		case DialogueWindow::DUKE:
			activeDialogueObject = dukeTextComponent->GetOwner().GetParent();
			dukeTextComponent->SetText(dialogue->text);
			break;
		case DialogueWindow::DOOR:
			activeDialogueObject = doorTextComponent->GetOwner().GetParent();
			doorTextComponent->SetText(dialogue->text);
			break;
		case DialogueWindow::TUTO_FANG:
			activeDialogueObject = tutorialFang;
			break;
		case DialogueWindow::TUTO_ONIMARU:
			activeDialogueObject = tutorialOnimaru;
			break;
		case DialogueWindow::TUTO_SWAP:
			activeDialogueObject = tutorialSwap;
			GameController::switchTutorialActive = true;
			break;
		case DialogueWindow::UPGRADES1:
			activeDialogueObject = tutorialUpgrades1;
			break;
		case DialogueWindow::UPGRADES2:
			activeDialogueObject = tutorialUpgrades2;
			break;
		case DialogueWindow::UPGRADES3:
			activeDialogueObject = tutorialUpgrades3;
			break;
		default:
			break;
		}

		runOpenAnimation = runAnimation;
		uiComponents.clear();
		uiColors.clear();
		RetrieveUIComponents(activeDialogueObject);
		GameController::isGameplayBlocked = dialogue->isBlocking;

		// Camera Zoom In
		if (cameraControllerScript) {
			cameraControllerScript->ZoomIn();
		}
	} else {
		activeDialogueObject = nullptr;
		GameController::isGameplayBlocked = false;
		GameController::switchTutorialActive = false;

		// Camera Zoom Out
		if (cameraControllerScript) {
			cameraControllerScript->ZoomOut();
		}
	}
}

void DialogueManager::ActivateDialogue(Dialogue* dialogue) {
	runSecondaryOpen = false;

	if (activeDialogueObject) {
		if (!activeDialogueObject->IsActive()) {
			activeDialogueObject->Enable();
		}
	}

	animationLerpTime += Time::GetDeltaTime();

	if (runOpenAnimation) {
		if (animationLerpTime < appearAnimationTime) {
			activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(float3::Lerp(currentStartPosition, currentEndPosition, animationLerpTime / appearAnimationTime));
			TransitionUIElementsColor(true);
		} else {
			runOpenAnimation = false;
			animationLerpTime = 0;
			activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(currentEndPosition);
			TransitionUIElementsColor(true, false);
		}
	}
}

void DialogueManager::ActivateNextDialogue(Dialogue* dialogue) {
	activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(currentStartPosition);
	activeDialogueObject->Disable();

	// TODO: here should go the transition animations
	SetActiveDialogue(dialogue->nextDialogue, false);
	activeDialogueObject->Enable();
	if (runSecondaryOpen) {
		runOpenAnimation = true;
	} else {
		activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(currentEndPosition);
	}
	runChangeAnimation = false;
}

void DialogueManager::CloseDialogue(Dialogue* dialogue) {
	animationLerpTime += Time::GetDeltaTime();
	if (runCloseAnimation) {
		if (animationLerpTime < appearAnimationTime) {
			activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(float3::Lerp(currentEndPosition, currentStartPosition, animationLerpTime / appearAnimationTime));
			TransitionUIElementsColor(false);
		} else {
			runCloseAnimation = false;
			animationLerpTime = 0;
			activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(currentStartPosition);
			TransitionUIElementsColor(true, false);

			if (activeDialogueObject) {
				if (activeDialogueObject->IsActive()) {
					activeDialogueObject->Disable();
				}
			}
			if (!dialogue->nextDialogue) SetActiveDialogue(nullptr);
		}
	}
}

void DialogueManager::ActivatePowerUpDialogue() {
	int obtainedPowerUps = playerControllerScript ? playerControllerScript->obtainedUpgradeCells : 0;
	int nextDialogue;
	switch (obtainedPowerUps) {
	case 1:
		nextDialogue = 0;
		break;
	case 2:
		nextDialogue = 4;
		break;
	case 3:
		nextDialogue = 5;
		break;
	default:
		return;
	}
	SetActiveDialogue(&dialoguesArray[nextDialogue]);
}

void DialogueManager::TransitionUIElementsColor(bool appearing, bool mustLerp) {
	int i = 0;
	float maximumTime = appearing ? appearAnimationTime : disappearAnimationTime;

	for (Component* component : uiComponents) {
		float4 originColor = appearing ? float4(uiColors[i].x, uiColors[i].y, uiColors[i].z, 0.0f) : uiColors[i];
		float4 targetColor = appearing ? uiColors[i] : float4(uiColors[i].x, uiColors[i].y, uiColors[i].z, 0.0f);
		if (component->GetType() == ComponentType::IMAGE) {
			static_cast<ComponentImage*>(component)->SetColor(float4::Lerp(originColor, targetColor, mustLerp ? (animationLerpTime / maximumTime) : 1.0f));
		} else {
			static_cast<ComponentText*>(component)->SetFontColor(float4::Lerp(originColor, targetColor, mustLerp ? (animationLerpTime / maximumTime) : 1.0f));
		}
		i++;
	}
}

void DialogueManager::RetrieveUIComponents(GameObject* current) {
	if (!current) return;

	ComponentImage* image = current->GetComponent<ComponentImage>();
	ComponentText* text = current->GetComponent<ComponentText>();
	if (image) {
		uiComponents.push_back(static_cast<Component*>(image));
		uiColors.push_back(image->GetMainColor());
	} else if (text) {
		uiComponents.push_back(static_cast<Component*>(text));
		uiColors.push_back(text->GetFontColor());
	}
	for (GameObject* child : current->GetChildren()) {
		RetrieveUIComponents(child);
	}
}
