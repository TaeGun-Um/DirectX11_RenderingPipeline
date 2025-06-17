#include "PrecompileHeader.h"
#include "SkyBoxActor.h"

#include <DirectX11_Extension/Ext_DirectXTexture.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Scene.h>
#include <DirectX11_Extension/Ext_Camera.h>

void SkyBoxActor::Start()
{
	// Ext_DirectXTexture::Find("TestReflection0");
	MeshComp = CreateComponent<Ext_MeshComponent>("SkyBox");
	MeshComp->CreateMeshComponentUnit("FullBox", "SkyBox");
	MeshComp->SetTexture("SkyBox", "CubeMapTex");

	// GetTransform()->SetLocalScale({ 100000.f, 100000.f, 100000.f });
	// GetTransform()->SetLocalScale({ 100.f, 100.f, 100.f });

	GetOwnerScene().lock()->SetSkyBox(GetSharedFromThis<Ext_Actor>());
}

void SkyBoxActor::Update(float _DeltaTime)
{
	// 항상 카메라 위치로 이동
	float4 CamPos = GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalPosition();
	GetTransform()->SetLocalPosition(CamPos);
}
