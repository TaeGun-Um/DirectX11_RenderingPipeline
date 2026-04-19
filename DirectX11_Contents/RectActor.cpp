#include "PrecompileHeader.h"
#include "RectActor.h"

void RectActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Rect", MaterialType::StaticAlpha);
	MeshComp->SetTexture("Manual.png");
	MeshComp->SetAlpha();

	GetTransform()->SetLocalScale({ 200.f, 200.f, 200.f });
}
