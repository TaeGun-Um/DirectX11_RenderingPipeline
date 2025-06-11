#include "PrecompileHeader.h"
#include "BoxActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>

void BoxActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Box", MaterialType::Static);
	MeshComp->SetTexture("Red.png");

	GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
}

void BoxActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);

	//if (bIsRotation)
	//{
	//	float MoveSpeed = 50.0f; // �ʴ� 100 ���� ȸ��
	//	MeshComp->GetTransform()->AddLocalRotation({ 0.f, MoveSpeed * _DeltaTime, 0.f });
	//}
}