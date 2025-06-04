#include "PrecompileHeader.h"
#include "TestPlatform.h"
#include <DirectX11_Extension/Ext_CollisionComponent.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

void TestPlatform::Start()
{
	PlatformBody = CreateComponent<Ext_CollisionComponent>("PlatformBody", static_cast<int>(CollisionGroup::Platform));
	PlatformBody->SetCollsionType(CollsionType::AABB3D);

	PlatformBodyMesh = CreateComponent<Ext_MeshComponent>("PlatformBodyMesh");
	PlatformBodyMesh->CreateMeshComponentUnit("Box", "StaticNonG");
	PlatformBodyMesh->SetTexture("Gray.png");

	PlatformBodyMesh->GetTransform()->SetLocalScale({ 300.f, 2.f, 300.f });
	PlatformBody->GetTransform()->SetLocalScale({ 300.f, 2.f, 300.f });
}

void TestPlatform::Update(float _DletaTime)
{


	__super::Update(_DletaTime);
}
