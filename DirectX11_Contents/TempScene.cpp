#include "PrecompileHeader.h"
#include "TempScene.h"
#include <DirectX11_Extension/Ext_Scene.h>
#include <DirectX11_Extension/Ext_Camera.h>
#include <DirectX11_Extension/Ext_Transform.h>

TempScene::TempScene()
{
}

TempScene::~TempScene()
{
}

void TempScene::Start()
{
	GetMainCamera()->GetTransform()->SetWorldPosition({ 0.f, 0.f, -100.0f });
	GetMainCamera()->GetTransform()->SetWorldRotation({ 0.f, 0.f, 0.f }); // Z+ 방향 바라보게
}

void TempScene::Update(float _DeltaTime)
{
	
	Ext_Scene::Update(_DeltaTime);
}

void TempScene::Destroy()
{

}