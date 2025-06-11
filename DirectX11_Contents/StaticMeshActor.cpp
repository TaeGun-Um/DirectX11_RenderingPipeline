#include "PrecompileHeader.h"
#include "StaticMeshActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>

void StaticMeshActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BookMesh");
	//MeshComp->CreateMeshComponentUnit("Book", MaterialType::PBR);
	//MeshComp->SetTexture("Book_BaseColor.jpg", TextureType::BaseColor);
	//MeshComp->SetTexture("Book_Normal.jpg", TextureType::Normal);
	//MeshComp->ShadowOn();
	MeshComp->CreateMeshComponentUnit("Book", MaterialType::Static);
	MeshComp->SetTexture("Book_BaseColor.jpg", TextureType::BaseColor);
	MeshComp->ShadowOn(ShadowType::Static);

	//GetTransform()->AddLocalRotation({ 0.f, 0.f, 90.f });
	//GetTransform()->AddLocalRotation({ 0.f, 90.f, 0.f });
	GetTransform()->SetLocalScale({ 2.5f, 2.5f, 2.5f });
}

void StaticMeshActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);

	if (bIsRotation)
	{
		float MoveSpeed = 50.0f; // 초당 100 단위 회전
		MeshComp->GetTransform()->AddLocalRotation({ MoveSpeed * _DeltaTime, 0.f, 0.f });
	}
}