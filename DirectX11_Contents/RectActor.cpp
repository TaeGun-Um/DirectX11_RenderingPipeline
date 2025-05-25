#include "PrecompileHeader.h"
#include "RectActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

RectActor::RectActor()
{
}

RectActor::~RectActor()
{
}

void RectActor::Start()
{
	Ext_Actor::Start();
	// CreateComponent<Ext_Component>("TestComp", true);
	GetTransform()->SetWorldPosition({ -100.f, 0.f, 0.0f });
	GetTransform()->SetWorldScale({ 50.f, 50.f, 50.f }); // 크기 확대
	std::shared_ptr<Ext_MeshComponent> MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh", true);
	MeshComp->CreateMeshComponentUnit("Rect", "Basic");
}

void RectActor::Update(float _DeltaTime)
{
	std::shared_ptr<Ext_Transform> Transform = GetTransform();
	GetTransform()->AddWorldRotation({ 0.f, 1.f, 0.f });
}

void RectActor::Destroy()
{

}