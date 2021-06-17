#pragma once

#include "Module.h"
#include "Utils/Buffer.h"

#include "Navigation/NavMesh.h"
#include "Resources/ResourceNavMesh.h"
#include "DetourCrowd/DetourCrowd.h"

class ModuleNavigation : public Module {
public:
	bool Init() override;								// Listens to PRESSED_PLAY and PRESSED_STOP events
	UpdateStatus Update() override;						// Updates agents of navMesh's crowd
	void ReceiveEvent(TesseractEvent& e) override;		// If PRESSED_PLAY Adds each agent. If PRESSED_STOP removes each agent.

	void BakeNavMesh();				// Builds new navMesh
	
	void DrawGizmos();				// Draws NavMesh Gizmos
	NavMesh& GetNavMesh();			// Returns navMesh

private:
	NavMesh navMesh;
};
