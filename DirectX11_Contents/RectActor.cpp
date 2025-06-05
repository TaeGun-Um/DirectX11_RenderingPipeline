#include "PrecompileHeader.h"
#include "RectActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>

void RectActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Rect", MaterialSetting::StaticNonG);
	// MeshComp->CreateMeshComponentUnit("Rect", "Static");
	MeshComp->SetTexture("Manual.png");

	GetTransform()->SetLocalScale({ 200.f, 200.f, 200.f });
}

void RectActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);

	if (bIsRotation)
	{
		float MoveSpeed = 50.0f; // 초당 100 단위 회전
		MeshComp->GetTransform()->AddLocalRotation({ 0.f, MoveSpeed * _DeltaTime, 0.f });
	}
}