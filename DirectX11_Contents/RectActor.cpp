#include "PrecompileHeader.h"
#include "RectActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Scene.h>
#include <DirectX11_Extension/Ext_Camera.h>
#include <DirectX11_Extension/Ext_DirectXDevice.h>
#include <DirectX11_Extension/Ext_DirectXConstantBuffer.h>

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
	GetTransform()->SetWorldPosition({ 0.f, 0.f, -100.0f });
	GetTransform()->SetWorldScale({ 100.f, 100.f, 1.f }); // 크기 확대
	std::shared_ptr<Ext_MeshComponent> MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh", true);
	MeshComp->CreateMeshComponentUnit("Rect", "Basic");
}

void RectActor::Update(float _DeltaTime)
{
	std::shared_ptr<Ext_Transform> Transform = GetTransform();
	// /*WorldMatrix*/Transform->GetWorldMatrix();
	// /*ViewMatrix*/GetOwnerScene().lock()->GetMainCamera()->GetViewMatrix();
	// /*ProjMatrix*/GetOwnerScene().lock()->GetMainCamera()->GetProjectionMatrix();

	GetTransform()->AddWorldRotation({ 0.f, 0.f, 1.f });

	float4 Pos = GetTransform()->GetWorldPosition();
	float4 Rot = GetTransform()->GetWorldRotation();
	float4 Scla = GetTransform()->GetWorldScale();
}

void RectActor::Destroy()
{

}