#include "PrecompileHeader.h"
#include "StoneWallActor.h"

#include <DirectX11_Extension/Ext_CollisionComponent.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_ReflectionComponent.h>

void StoneWallActor::Start()
{
	GetTransform()->SetLocalScale({ 300.f, 300.f, 1.f });

	WallBodyMesh = CreateComponent<Ext_MeshComponent>("WallBodyMesh");
	WallBodyMesh->CreateMeshComponentUnit("Rect", MaterialType::StaticCUBE);
	WallBodyMesh->SetTexture("Gray.png");
}

void StoneWallActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);
}

void StoneWallActor::SetReflection()
{
	Reflection = Ext_ReflectionComponent::AttachTo(GetSharedFromThis<Ext_Actor>(), WallBodyMesh);
}
