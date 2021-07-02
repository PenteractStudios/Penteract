#include "DialogueManager.h"
#include "DialogueManager.h"

#include "GameplaySystems.h"
#include "GameObject.h"
#include "Components/UI/ComponentText.h"

EXPOSE_MEMBERS(DialogueManager) {
	MEMBER(MemberType::GAME_OBJECT_UID, fangTextObjectUID),
	MEMBER(MemberType::GAME_OBJECT_UID, onimaruTextObjectUID),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialFangUID),
	MEMBER(MemberType::GAME_OBJECT_UID, tutorialOnimaruUID),
	MEMBER(MemberType::FLOAT3, dialogueStartPosition),
	MEMBER(MemberType::FLOAT3, dialogueEndPosition),
	MEMBER(MemberType::FLOAT3, tutorialStartPosition),
	MEMBER(MemberType::FLOAT3, tutorialEndPosition),
	MEMBER(MemberType::FLOAT, appearAnimationTime),
	MEMBER(MemberType::FLOAT, disappearAnimationTime)
};

GENERATE_BODY_IMPL(DialogueManager);

void DialogueManager::Start() {
	GameObject* fangTextObject = GameplaySystems::GetGameObject(fangTextObjectUID);
	GameObject* onimaruTextObject = GameplaySystems::GetGameObject(onimaruTextObjectUID);
	tutorialFang = GameplaySystems::GetGameObject(tutorialFangUID);
	tutorialOnimaru = GameplaySystems::GetGameObject(tutorialOnimaruUID);

	if (fangTextObject && onimaruTextObject) {
		fangTextComponent = fangTextObject->GetComponent<ComponentText>();
		onimaruTextComponent = onimaruTextObject->GetComponent<ComponentText>();
	}

	// SWAP DIALOGUE + ONIMARU TUTORIAL
	dialoguesArray[0] = Dialogue(1, "Hi, I'm Fang!\n akljafddoadf\ndlskds\nldsjds", &dialoguesArray[1]);
	dialoguesArray[1] = Dialogue(2, "And I'm Onimaru", &dialoguesArray[2]);
	dialoguesArray[2] = Dialogue(2, "N' we gonna kik yur arse!", &dialoguesArray[3]);
	dialoguesArray[3] = Dialogue(5, "", &dialoguesArray[4]);
	dialoguesArray[4] = Dialogue(2, "And I'm Onimaru", nullptr);

}

void DialogueManager::Update() {
	if (!fangTextComponent) return;
	if (!onimaruTextComponent) return;
	if (!tutorialFang) return;
	if (!tutorialOnimaru) return;

	if (activeDialogue) {
		if (runOpenAnimation) ActivateDialogue(activeDialogue);

		if (Input::GetKeyCodeDown(Input::KEY_F) && !(runOpenAnimation || runChangeAnimation || runCloseAnimation) && activeDialogueObject) {
			if (activeDialogue->nextDialogue) {
				runChangeAnimation = true;
				if ((activeDialogue->character < 5 && activeDialogue->nextDialogue->character >= 5) ||
					(activeDialogue->character >= 5 && activeDialogue->nextDialogue->character < 5)) {
					runCloseAnimation = true;
					runSecondaryOpen = true;
				}
			}
			else runCloseAnimation = true;
		}

		if (runChangeAnimation && !runCloseAnimation) {
			ActivateNextDialogue(activeDialogue);
		}

		if (runCloseAnimation) CloseDialogue(activeDialogue);
	}
}

void DialogueManager::SetActiveDialogue(Dialogue* dialogue, bool runAnimation)
{
	if (dialogue) {
		if (dialogue->character >= 5) {
			currentStartPosition = tutorialStartPosition;
			currentEndPosition = tutorialEndPosition;
		}
		else {
			currentStartPosition = dialogueStartPosition;
			currentEndPosition = dialogueEndPosition;
		}

		activeDialogue = dialogue;
		if (dialogue->character == 1) {          // FANG
			activeDialogueObject = fangTextComponent->GetOwner().GetParent();
			fangTextComponent->SetText(dialogue->text);
		}
		else if (dialogue->character == 2) {     // ONIMARU
			activeDialogueObject = onimaruTextComponent->GetOwner().GetParent();
			onimaruTextComponent->SetText(dialogue->text);
		}
		else if (dialogue->character == 3) {     // DUKE
		}
		else if (dialogue->character == 4) {     // ROSAMONDE
		}
		else if (dialogue->character == 5) {     // TUTORIAL
			activeDialogueObject = tutorialFang;
		}
		else if (dialogue->character == 6) {     // TUTORIAL
			activeDialogueObject = tutorialOnimaru;
		}
		runOpenAnimation = runAnimation;
		RetrieveUIComponents(activeDialogueObject);
	}
	else {
		//activeDialogue = nullptr;
		activeDialogueObject = nullptr;
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
		}
		else {
			runOpenAnimation = false;
			animationLerpTime = 0;
			activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(currentEndPosition);
			TransitionUIElementsColor(true, false);
		}
	}
}

void DialogueManager::ActivateNextDialogue(Dialogue* dialogue)
{
	activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(currentStartPosition);
	activeDialogueObject->Disable();
	// here should go the transition animations
	SetActiveDialogue(dialogue->nextDialogue, false);
	activeDialogueObject->Enable();
	if (runSecondaryOpen) {
		runOpenAnimation = true;
	}
	else {
		activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(currentEndPosition);
	}
	runChangeAnimation = false;
}

void DialogueManager::CloseDialogue(Dialogue* dialogue)
{
	animationLerpTime += Time::GetDeltaTime();
	if (runCloseAnimation) {
		if (animationLerpTime < appearAnimationTime) {
			activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(float3::Lerp(currentEndPosition, currentStartPosition, animationLerpTime / appearAnimationTime));
			TransitionUIElementsColor(false);
		}
		else {
			runCloseAnimation = false;
			animationLerpTime = 0;
			activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(currentStartPosition);
			TransitionUIElementsColor(false, false);

			if (activeDialogueObject) {
				if (activeDialogueObject->IsActive()) {
					activeDialogueObject->Disable();
				}
			}
			if (!dialogue->nextDialogue) SetActiveDialogue(nullptr);
		}
	}
}

void DialogueManager::TransitionUIElementsColor(bool appearing, bool mustLerp)
{
	int i = 0;
	float maximumTime = appearing ? appearAnimationTime : disappearAnimationTime;

	for (Component* component : uiComponents) {
		float4 originColor = appearing ? float4(uiColors[i].x, uiColors[i].y, uiColors[i].z, 0.0f) : uiColors[i];
		float4 targetColor = appearing ? uiColors[i] : float4(uiColors[i].x, uiColors[i].y, uiColors[i].z, 0.0f);
		if (component->GetType() == ComponentType::IMAGE) {
			static_cast<ComponentImage*>(component)->SetColor(float4::Lerp(originColor, targetColor, mustLerp ? (animationLerpTime / maximumTime) : 1.0f));
		}
		else {
			static_cast<ComponentText*>(component)->SetFontColor(float4::Lerp(originColor, targetColor, mustLerp ? (animationLerpTime / maximumTime) : 1.0f));
		}
		i++;
	}
}

void DialogueManager::RetrieveUIComponents(GameObject* current)
{
	if (!current) return;

	uiComponents.clear();
	uiColors.clear();

	ComponentImage* image = current->GetComponent<ComponentImage>();
	ComponentText* text = current->GetComponent<ComponentText>();
	if (image) {
		uiComponents.push_back(static_cast<Component*>(image));
		uiColors.push_back(image->GetMainColor());
	}
	else if (text) {
		uiComponents.push_back(static_cast<Component*>(text));
		uiColors.push_back(text->GetFontColor());
	}
	for (GameObject* child : current->GetChildren()) {
		RetrieveUIComponents(child);
	}
}


// ------------------------------------------ DIALOGUE
DialogueManager::Dialogue::Dialogue()
{
}

DialogueManager::Dialogue::Dialogue(int character_, const char* text_, Dialogue* nextDialogue_)
{
	character = character_;
	text = text_;
	nextDialogue = nextDialogue_;
}

DialogueManager::Dialogue::~Dialogue()
{
}
