#include "PrecompileHeader.h"
#include "RectActor.h"

void RectActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Rect", MaterialType::NonGStatic);
	MeshComp->SetTexture("Manual.png");
	MeshComp->ShadowOn(ShadowType::Static);

	GetTransform()->SetLocalScale({ 200.f, 200.f, 200.f });
}
