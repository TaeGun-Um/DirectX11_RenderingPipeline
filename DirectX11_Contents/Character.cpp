#include "PrecompileHeader.h"
#include "Character.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_FBXMeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>
#include <DirectX11_Base/Base_Math.h>

void Character::Start()
{
	GetTransform()->SetLocalPosition({ 0, 0, 5 });
	GetTransform()->SetLocalScale({ 0.05f, 0.05f, 0.05f });

	MeshComp = CreateComponent<Ext_FBXMeshComponent>("BodyMesh");
	MeshComp->CreateMeshComponentUnit("Girl", "Animation");
	MeshComp->SetTexture("Ch03_1001_Diffuse.png", "BaseColor");
}

void Character::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);
}