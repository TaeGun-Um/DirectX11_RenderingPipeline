#include "PrecompileHeader.h"
#include "TestScene.h"

#include "TriangleActor.h"
#include "RectActor.h"

TestScene::TestScene()
{
}

TestScene::~TestScene()
{
}

void TestScene::Start()
{
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
