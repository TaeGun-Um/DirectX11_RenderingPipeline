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

	GetTransform()->SetLocalPosition({ 0.f, 10.f, 10.f });
	//GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });

	//MeshComp->GetTransform()->SetWorldPosition({ 0.f, 20.f, 0.f });
}

void BoxActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);

	AccTime += _DeltaTime;

	if (AccTime > 3.0f)
	{
		float MoveSpeed = 10.0f;
		GetTransform()->AddLocalPosition({ 0.0f, 0.0f, MoveSpeed * _DeltaTime });
	}

	//if (bIsRotation)
	//{
	//	float MoveSpeed = 50.0f; // 초당 100 단위 회전
	//	MeshComp->GetTransform()->AddLocalRotation({ 0.f, MoveSpeed * _DeltaTime, 0.f });
	//}


}