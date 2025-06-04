#include "PrecompileHeader.h"
#include "TestScene.h"

#include "CubeActor.h"
#include "SphereActor.h"
#include "CylinderActor.h"
#include "StaticMeshActor.h"
#include "Character.h"
#include "TestPlatform.h"
#include <DirectX11_Extension/Ext_Scene.h>
#include <DirectX11_Extension/Ext_Camera.h>
#include <DirectX11_Extension/Ext_Transform.h>

TestScene::TestScene()
{
}

TestScene::~TestScene()
{
}

void TestScene::Start()
{
	CreateActor<Character>("Character");
	CreateActor<TestPlatform>("TestPlatform");

	std::shared_ptr<CubeActor> CubeActor1 = CreateActor<CubeActor>("CubeActor1");
	CubeActor1->GetTransform()->SetLocalPosition({-5.f, 7.f, 10.f});

	std::shared_ptr<CubeActor> CubeActor2 = CreateActor<CubeActor>("CubeActor2");
	CubeActor2->GetTransform()->SetLocalPosition({ -5.f, 10.f, 10.f });
	CubeActor2->SetRotate();

	std::shared_ptr<SphereActor> SphereActor1 = CreateActor<SphereActor>("SphereActor1");
	SphereActor1->GetTransform()->SetLocalPosition({ 0.f, 7.f, 10.f });
	
	std::shared_ptr<SphereActor> SphereActor2 = CreateActor<SphereActor>("SphereActor2");
	SphereActor2->GetTransform()->SetLocalPosition({ 0.f, 10.f, 10.f });
	SphereActor2->SetRotate();

	std::shared_ptr<CylinderActor> CylinderActor1 = CreateActor<CylinderActor>("CylinderActor1");
	CylinderActor1->GetTransform()->SetLocalPosition({ 5.f, 7.f, 10.f });

	std::shared_ptr<CylinderActor> CylinderActor2 = CreateActor<CylinderActor>("CylinderActor2");
	CylinderActor2->GetTransform()->SetLocalPosition({ 5.f, 10.f, 10.f });
	CylinderActor2->SetRotate();

	std::shared_ptr<StaticMeshActor> StaticMeshActor1 = CreateActor<StaticMeshActor>("StaticMeshActor1");
	StaticMeshActor1->GetTransform()->SetLocalPosition({ 10.f, 7.f, 10.f });

	std::shared_ptr<StaticMeshActor> StaticMeshActor2 = CreateActor<StaticMeshActor>("StaticMeshActor2");
	StaticMeshActor2->GetTransform()->SetLocalPosition({ 10.f, 14.f, 10.f });
	StaticMeshActor2->SetRotate();
}

void TestScene::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);
}

