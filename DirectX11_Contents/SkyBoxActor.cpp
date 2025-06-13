#include "PrecompileHeader.h"
#include "SkyBoxActor.h"

#include <DirectX11_Extension/Ext_DirectXTexture.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

void SkyBoxActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("SkyBox");
	MeshComp->CreateMeshComponentUnit("FullBox", "SkyBox");
	MeshComp->SetTexture("SkyBox", "CubeMapTex");

	GetTransform()->SetLocalScale({ 1000.f, 1000.f, 1000.f });
}

void SkyBoxActor::Update(float _DeltaTime)
{

}
