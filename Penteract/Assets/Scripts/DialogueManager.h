#pragma once

#include "Scripting/Script.h"

class ComponentText;
class DialogueManager : public Script
{
	GENERATE_BODY(DialogueManager);

	class Dialogue {
	public:
		Dialogue();
		Dialogue(int character, const char* text, Dialogue* nextDialogue);
		~Dialogue();
	public:
		int character = 0; // 1 = Fang, 2 = Onimaru, 3 = Duke, 4 = Rosamonde, 5 = Tutorial Window
		const char* text = nullptr;
		Dialogue* nextDialogue = nullptr;
	};

public:

	void Start() override;
	void Update() override;

	void SetActiveDialogue(Dialogue* dialogue, bool runAnimation = true);
	void ActivateDialogue(Dialogue* dialogue);
	void ActivateNextDialogue(Dialogue* dialogue);
	void CloseDialogue(Dialogue* dialogue);

	void TransitionUIElementsColor(bool appearing = true, bool mustLerp = true);
	void RetrieveUIComponents(GameObject* current);

public:
	Dialogue dialoguesArray[5];
	Dialogue* activeDialogue = nullptr;			// Pointer to the Dialogue Tree
	GameObject* activeDialogueObject = nullptr; // Pointer to the Dialogue GameObject in the scene (DialogueFang, DialogueOnimaru or DialogueDuke)

	// ------ GAMEOBJECT REFS ------ //
	UID fangTextObjectUID = 0;
	UID onimaruTextObjectUID = 0;
	ComponentText* fangTextComponent = 0;
	ComponentText* onimaruTextComponent = 0;

	UID tutorialFangUID = 0;
	UID tutorialOnimaruUID = 0;
	GameObject* tutorialFang = nullptr;
	GameObject* tutorialOnimaru = nullptr;

	// ------ TRANSITION INFO ------ //
	float3 dialogueStartPosition = float3(0, 0, 0);
	float3 dialogueEndPosition = float3(0, 0, 0);
	float3 tutorialStartPosition = float3(0, 0, 0);
	float3 tutorialEndPosition = float3(0, 0, 0);
	float3 currentStartPosition = float3(0, 0, 0);
	float3 currentEndPosition = float3(0, 0, 0);
	float appearAnimationTime = .5f;
	float disappearAnimationTime = .5f;
	float animationLerpTime = 0.0f;
	std::vector<Component*> uiComponents;
	std::vector<float4> uiColors;
	bool runOpenAnimation = false;
	bool runChangeAnimation = false;
	bool runCloseAnimation = false;
	bool runSecondaryOpen = false;

	// ---------- TRIGGERS --------- //
	bool triggerSwapDialogues = false;
};
