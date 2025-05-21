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
	GetTransform()->SetWorldPosition({ 0.f, 0.f, 10.0f });
	GetTransform()->SetWorldScale({ 10.f, 10.f, 1.f }); // 크기 확대
}

void RectActor::Update(float _DeltaTime)
{
	std::shared_ptr<Ext_Transform> Transform = GetTransform();
	// /*WorldMatrix*/Transform->GetWorldMatrix();
	// /*ViewMatrix*/GetOwnerScene().lock()->GetMainCamera()->GetViewMatrix();
	// /*ProjMatrix*/GetOwnerScene().lock()->GetMainCamera()->GetProjectionMatrix();

	GetTransform()->AddWorldRotation({ 0.f, 0.f, 1.f });
}

void RectActor::Destroy()
{

}