#include "LavaFlow.h"

#include "GameObject.h"
#include "Components/ComponentMeshRenderer.h"
#include "RandomNumberGenerator.h"
#include "GameplaySystems.h"

EXPOSE_MEMBERS(LavaFlow) {
     MEMBER(MemberType::FLOAT, speed),
     MEMBER(MemberType::BOOL, randomStart),
};

GENERATE_BODY_IMPL(LavaFlow);

void LavaFlow::Start() {
    mesh = GetOwner().GetComponent<ComponentMeshRenderer>();
    if (randomStart && mesh) {
        float2 offset = mesh->GetTextureOffset();
        offset.y += RandomNumberGenerator::GenerateFloat(0.0f, 1.0f);
        mesh->SetTextureOffset(offset);
    }
}

void LavaFlow::Update() {
    if (mesh) {
        float2 offset = mesh->GetTextureOffset();
        offset.y += speed * Time::GetDeltaTime();
        mesh->SetTextureOffset(offset);
    }
}