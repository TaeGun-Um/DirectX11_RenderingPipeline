#include "PrecompileHeader.h"
#include "SphereActor.h"

void SphereActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Sphere", MaterialType::StaticAlpha);
	MeshComp->SetTexture("Gray.png");
	MeshComp->SetAlpha();

	GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
}
