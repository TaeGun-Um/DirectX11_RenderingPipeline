#include "PrecompileHeader.h"
#include "StaticMeshActor.h"

void StaticMeshActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BookMesh");
	MeshComp->CreateMeshComponentUnit("Book", MaterialType::Static);
	MeshComp->SetTexture("Book_BaseColor.jpg", TextureType::BaseColor);
	MeshComp->ShadowOn(ShadowType::Static);

	SetRotationAxis({ 1.f, 0.f, 0.f, 0.f }); // X축 회전
	GetTransform()->SetLocalScale({ 2.5f, 2.5f, 2.5f });
}
