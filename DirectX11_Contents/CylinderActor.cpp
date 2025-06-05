#include "PrecompileHeader.h"
#include "CylinderActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>

void CylinderActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	//MeshComp->CreateMeshComponentUnit("Cylinder", "StaticNonG");
	MeshComp->CreateMeshComponentUnit("Cylinder", MaterialSetting::Static);
	MeshComp->SetTexture("Green.png");

	GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
}

void CylinderActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);

	if (bIsRotation)
	{
		float MoveSpeed = 50.0f; // �ʴ� 100 ���� ȸ��
		MeshComp->GetTransform()->AddLocalRotation({ 0.f, MoveSpeed * _DeltaTime, 0.f });
	}
}
