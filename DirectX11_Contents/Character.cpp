#include "PrecompileHeader.h"
#include "Character.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>
#include <DirectX11_Base/Base_Math.h>

void Character::Start()
{
	GetTransform()->SetLocalPosition({ 0, 0, 5 });
	GetTransform()->SetLocalScale({ 0.1, 0.1, 0.1 });

	MeshComp = CreateComponent<Ext_MeshComponent>("BodyMesh");
	MeshComp->CreateMeshComponentUnit("Belorian Soldier LP", "Basic");
}

void Character::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);
}