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
	PlatformBodyMesh->CreateMeshComponentUnit("Box", MaterialType::Static);
	PlatformBodyMesh->SetSampler(SamplerType::PointMirror);
	PlatformBodyMesh->SetTexture("Red.png");
	//PlatformBodyMesh->ShadowOn();

	GetTransform()->SetLocalScale({ 2000.f, 50.f, 2000.f });
	GetTransform()->SetLocalPosition({ 0.f, -25.f, 0.f });
}

void TestPlatform::Update(float _DletaTime)
{


	__super::Update(_DletaTime);
}
