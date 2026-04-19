#include "PrecompileHeader.h"
#include "CylinderActor.h"

void CylinderActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Cylinder", MaterialType::Static);
	MeshComp->SetTexture("Gray.png");
	MeshComp->ShadowOn(ShadowType::Static);

	GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
}
