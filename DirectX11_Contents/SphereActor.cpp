#include "PrecompileHeader.h"
#include "SphereActor.h"

void SphereActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Sphere", MaterialType::Static);
	MeshComp->SetTexture("Gray.png");
	MeshComp->ShadowOn(ShadowType::Static);
	MeshComp->SetMask();

	GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
}
