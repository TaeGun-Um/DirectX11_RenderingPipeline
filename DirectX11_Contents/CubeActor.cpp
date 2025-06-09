#include "PrecompileHeader.h"
#include "CubeActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>

void CubeActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	//MeshComp->CreateMeshComponentUnit("Box", MaterialType::PBR);
	//MeshComp->SetTexture("StoneWall_BaseColor.jpg");
	//MeshComp->SetTexture("StoneWall_Normal.jpg", TextureType::Normal);
	//MeshComp->ShadowOn();
	MeshComp->CreateMeshComponentUnit("Box", MaterialType::Static);
	MeshComp->SetTexture("Gray.png");
	MeshComp->ShadowOn();

	GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
}

void CubeActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);

	if (bIsRotation)
	{
		float MoveSpeed = 50.0f; // 초당 100 단위 회전
		MeshComp->GetTransform()->AddLocalRotation({ 0.f, MoveSpeed * _DeltaTime, 0.f });
	}
}