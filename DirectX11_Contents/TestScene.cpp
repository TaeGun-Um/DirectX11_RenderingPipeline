#include "PrecompileHeader.h"
#include "TestScene.h"

#include "RectActor.h"
#include "CubeActor.h"
#include "TestActor.h"
#include "Character.h"
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
	GetMainCamera()->GetTransform()->SetLocalRotation({ 0.f, 0.f, 0.f }); // Z+ 방향 바라보게
	// CreateActor<CubeActor>("CubeActor");
	// CreateActor<RectActor>("RectActor");
	// CreateActor<TestActor>("TestActor");

	CreateActor<Character>("Character");
}

float AccTime = 0;
bool one = false;

void TestScene::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);
}

