#include "PrecompileHeader.h"
#include "RotateFace.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

RotateFace::RotateFace()
{
}

RotateFace::~RotateFace()
{
}

void RotateFace::Start()
{
	Ext_Actor::Start();
	// CreateComponent<Ext_Component>("TestComp", true);
	GetTransform()->SetWorldPosition({ 0.f, 0.f, 10.0f });
	GetTransform()->SetWorldScale({ 100.f, 100.f, 1.f }); // 크기 확대
	std::shared_ptr<Ext_MeshComponent> MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh", true);
	MeshComp->CreateMeshComponentUnit("RotateFace", "Basic");
}

void RotateFace::Update(float _DeltaTime)
{
	std::shared_ptr<Ext_Transform> Transform = GetTransform();

	GetTransform()->AddWorldRotation({ 0.f, 5.f, 0.f });
	// GetTransform()->AddWorldRotation({ 5.f, 0.f, 0.f });
	// GetTransform()->AddWorldRotation({ 0.f, 0.f, 5.f });
	// GetTransform()->AddWorldPosition({ 0.f, 0.1f, 0.3f });
}

void RotateFace::Destroy()
{

}