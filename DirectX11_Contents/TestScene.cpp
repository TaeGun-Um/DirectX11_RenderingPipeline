#include "PrecompileHeader.h"
#include "TestScene.h"

#include "TriangleActor.h"
#include "RectActor.h"
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
	GetMainCamera()->GetTransform()->SetWorldPosition({ 0.f, 0.f, -100.0f });
	GetMainCamera()->GetTransform()->SetWorldRotation({ 0.f, 0.f, 0.f }); // Z+ 방향 바라보게

	CreateActor<TriangleActor>("TriangleActor");
	CreateActor<RectActor>("RectActor");
}

void TestScene::Update(float _DeltaTime)
{
	Ext_Scene::Update(_DeltaTime);
}

void TestScene::Destroy()
{

}
