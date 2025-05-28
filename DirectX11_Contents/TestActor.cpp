#include "PrecompileHeader.h"
#include "TestActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

void TestActor::Start()
{
	GetTransform()->SetLocalPosition({ -3, 0, 2.5 });

	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Rect", "Basic");
	MeshComp->SetTexture("Cuphead.png");
}

void TestActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);

	float MoveSpeed = 1.0f; // �ʴ� 100 ���� ȸ��
	GetTransform()->AddLocalPosition({ MoveSpeed * _DeltaTime, 0.f , 0.f });
}
