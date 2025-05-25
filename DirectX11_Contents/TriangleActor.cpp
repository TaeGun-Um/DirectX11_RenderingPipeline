#include "PrecompileHeader.h"
#include "TriangleActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

TriangleActor::TriangleActor()
{
}

TriangleActor::~TriangleActor()
{
}

void TriangleActor::Start()
{
	GetTransform()->SetLocalPosition({ -200.f, 0.f, 200.0f });
	GetTransform()->SetLocalScale({ 100.f, 100.f, 100.f }); // 크기 확대

	std::shared_ptr<Ext_MeshComponent> MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Triangle", "Basic");
}

void TriangleActor::Update(float _DeltaTime)
{
	// 매프레임마다 Z축 기준 반시계 회전
	GetTransform()->AddLocalRotation({ 0.f, 0.f, -1.f });
}
