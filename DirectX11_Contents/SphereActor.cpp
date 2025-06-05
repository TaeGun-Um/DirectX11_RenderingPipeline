#include "PrecompileHeader.h"
#include "SphereActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>

void SphereActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	//MeshComp->CreateMeshComponentUnit("Sphere", "StaticNonG");
	MeshComp->CreateMeshComponentUnit("Sphere", MaterialType::Static);
	MeshComp->SetTexture("Gray.png");

	GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
}

void SphereActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);

	if (bIsRotation)
	{
		float MoveSpeed = 50.0f; // 초당 100 단위 회전
		MeshComp->GetTransform()->AddLocalRotation({ 0.f, MoveSpeed * _DeltaTime, 0.f });
	}
}