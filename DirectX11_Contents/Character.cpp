#include "PrecompileHeader.h"
#include "Character.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>
#include <DirectX11_Base/Base_Math.h>

void Character::Start()
{
	GetTransform()->SetLocalPosition({ 0, 0, 5 });
	GetTransform()->SetLocalScale({ 0.05f, 0.05f, 0.05f });

	MeshComp = CreateComponent<Ext_MeshComponent>("BodyMesh");
	MeshComp->CreateMeshComponentUnit("Ch43", "Basic");
	MeshComp->SetTexture("Ch43_1001_Diffuse.png", "BaseColor");
	// MeshComp->SetTexture("Ch43_1001_Normal.png", "NormalTex");
	// MeshComp->SetTexture("Ch43_1001_Glossiness.png", "RoughnessTex");
	// MeshComp->SetTexture("Ch43_1001_Specular.png", "MetallicTex");
}

void Character::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);
}