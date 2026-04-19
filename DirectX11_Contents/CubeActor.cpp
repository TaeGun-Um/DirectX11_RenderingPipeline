#include "PrecompileHeader.h"
#include "CubeActor.h"

void CubeActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Box", MaterialType::Static);
	MeshComp->SetTexture("Green.png");
	MeshComp->ShadowOn(ShadowType::Static);

	GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
}
