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
	GetTransform()->SetWorldPosition({ -200.f, 0.f, 200.0f });
	GetTransform()->SetWorldScale({ 100.f, 100.f, 100.f }); // 크기 확대
	std::shared_ptr<Ext_MeshComponent> MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh", true);
	MeshComp->CreateMeshComponentUnit("Triangle", "Basic");
}

void TriangleActor::Update(float _DeltaTime)
{
	std::shared_ptr<Ext_Transform> Transform = GetTransform();
	GetTransform()->AddWorldRotation({ 0.f, 0.f, -1.f });
}

void TriangleActor::Destroy()
{

}