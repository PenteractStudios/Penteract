#include "DialogueManager.h"
#include "DialogueManager.h"

#include "GameplaySystems.h"
#include "GameObject.h"
#include "Components/UI/ComponentText.h"

EXPOSE_MEMBERS(DialogueManager) {
	// Add members here to expose them to the engine. Example:
	// MEMBER(MemberType::BOOL, exampleMember1),
	// MEMBER(MemberType::PREFAB_RESOURCE_UID, exampleMember2),
	MEMBER(MemberType::GAME_OBJECT_UID, fangTextObjectUID),
		MEMBER(MemberType::GAME_OBJECT_UID, onimaruTextObjectUID),
		MEMBER(MemberType::FLOAT3, startPosition),
		MEMBER(MemberType::FLOAT3, endPosition),
		MEMBER(MemberType::FLOAT, appearAnimationTime),
		MEMBER(MemberType::FLOAT, disappearAnimationTime)
};

GENERATE_BODY_IMPL(DialogueManager);

void DialogueManager::Start() {
	GameObject* fangTextObject = GameplaySystems::GetGameObject(fangTextObjectUID);
	GameObject* onimaruTextObject = GameplaySystems::GetGameObject(onimaruTextObjectUID);
	if (fangTextObject && onimaruTextObject) {
		fangTextComponent = fangTextObject->GetComponent<ComponentText>();
		onimaruTextComponent = onimaruTextObject->GetComponent<ComponentText>();
		RetrieveUIComponents(fangTextComponent->GetOwner().GetParent());

		if (fangTextComponent && onimaruTextComponent) {
			dialoguesArray[0] = Dialogue(1, "Hi, I'm Fang!", &dialoguesArray[1]);
			dialoguesArray[1] = Dialogue(2, "And I'm Onimaru", &dialoguesArray[2]);
			dialoguesArray[2] = Dialogue(2, "N' we gonna kik yur arse!", nullptr);
		}
	}
}

void DialogueManager::Update() {
	if (!fangTextComponent) return;
	if (!onimaruTextComponent) return;

	// SWAP DIALOGUE + TUTORIAL
	if (triggerSwapDialogues) {

	}


	if (activeDialogue) {
		if (runOpenAnimation) ActivateDialogue(activeDialogue);

		if (Input::GetKeyCodeDown(Input::KEY_F) && !(runOpenAnimation || runChangeAnimation)) {
			if (activeDialogue->nextDialogue) runChangeAnimation = true;
			else runCloseAnimation = true;
		}

		if (runChangeAnimation) {
			ActivateNextDialogue(activeDialogue);
		}

		if (runCloseAnimation) CloseDialogue(activeDialogue);
	}
}

void DialogueManager::SetActiveDialogue(Dialogue* dialogue, bool runAnimation)
{
	if (dialogue) {
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
		runOpenAnimation = runAnimation;
	}
	else {
		activeDialogue = nullptr;
		activeDialogueObject = nullptr;
	}
}

void DialogueManager::ActivateDialogue(Dialogue* dialogue) {

	if (activeDialogueObject) {
		if (!activeDialogueObject->IsActive()) {
			activeDialogueObject->Enable();
		}
	}

	animationLerpTime += Time::GetDeltaTime();

	if (runOpenAnimation) {
		if (animationLerpTime < appearAnimationTime) {
			activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(float3::Lerp(startPosition, endPosition, animationLerpTime / appearAnimationTime));
			TransitionUIElementsColor(true);
		}
		else {
			runOpenAnimation = false;
			animationLerpTime = 0;
			activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(endPosition);
			TransitionUIElementsColor(true, false);
		}
	}
}

void DialogueManager::ActivateNextDialogue(Dialogue* dialogue)
{
	activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(startPosition);
	activeDialogueObject->Disable();

	// here should go the transition animations
	SetActiveDialogue(dialogue->nextDialogue, false);
	activeDialogueObject->Enable();
	activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(endPosition);
	runChangeAnimation = false;
}

void DialogueManager::CloseDialogue(Dialogue* dialogue)
{
	animationLerpTime += Time::GetDeltaTime();
	if (runCloseAnimation) {
		if (animationLerpTime < appearAnimationTime) {
			activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(float3::Lerp(endPosition, startPosition, animationLerpTime / appearAnimationTime));
			TransitionUIElementsColor(false);
		}
		else {
			runCloseAnimation = false;
			animationLerpTime = 0;
			activeDialogueObject->GetComponent<ComponentTransform2D>()->SetPosition(startPosition);
			TransitionUIElementsColor(false, false);

			
			if (activeDialogueObject) {
				if (activeDialogueObject->IsActive()) {
					activeDialogueObject->Disable();
				}
			}
			SetActiveDialogue(nullptr);
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
	if (current == nullptr) {
		return;
	}

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
