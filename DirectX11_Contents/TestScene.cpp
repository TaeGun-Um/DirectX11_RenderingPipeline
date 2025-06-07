#include "PrecompileHeader.h"
#include "TestScene.h"

#include "CubeActor.h"
#include "SphereActor.h"
#include "CylinderActor.h"
#include "StaticMeshActor.h"
#include "Character.h"
#include "RectActor.h"
#include "TestPlatform.h"
#include "StoneWallActor.h"

#include "InformationGUI.h"

#include <DirectX11_Extension/Ext_DirectXDevice.h>
#include <DirectX11_Extension/RendertargetGUI.h>
#include <DirectX11_Extension/Ext_Scene.h>
#include <DirectX11_Extension/Ext_Camera.h>
#include <DirectX11_Extension/Ext_Transform.h>

#include <DirectX11_Base/Base_Directory.h>
#include <DirectX11_Extension/Ext_DirectXMesh.h>
#include <DirectX11_Extension/Ext_DirectXTexture.h>
#include <DirectX11_Extension/Ext_Imgui.h>

TestScene::TestScene()
{
}

TestScene::~TestScene()
{
}

void TestScene::Start()
{
	// GUI 생성
	{
		Ext_Imgui::CreateImgui<InformationGUI>("InformationGUI");
		Ext_Imgui::CreateImgui<RendertargetGUI>("RendertargetGUI");
		RendertargetGUI::Clear();
		RendertargetGUI::AddDebugRenderTarget(0, "Shadow RenderTarget", GetDirectionalLight()->GetShadowRenderTarget());
		RendertargetGUI::AddDebugRenderTarget(1, "MainRenderTarget", Ext_DirectXDevice::GetMainRenderTarget());
	}

	// 캐릭터 메시 로드
	{
		Base_Directory Dir1;
		Dir1.MakePath("../Resource/Character/Mesh/Girl.fbx");
		Ext_DirectXMesh::CreateDynamicMesh(Dir1.GetPath());

		Base_Directory Dir2;
		Dir2.MakePath("../Resource/Character/Texture");
		std::vector<std::string> Paths = Dir2.GetAllFile({ "png", "tga", "dss" });
		for (const std::string& FilePath : Paths)
		{
			Ext_DirectXTexture::LoadTexture(FilePath.c_str());
		}
	}

	// 컨텐츠용 메시 로드
	{
		Base_Directory Dir1;
		Dir1.MakePath("../Resource/StaticMesh/Book/Mesh/Book.fbx");
		Ext_DirectXMesh::CreateStaticMesh(Dir1.GetPath());

		Base_Directory Dir2;
		Dir2.MakePath("../Resource/StaticMesh/Book/Texture");
		std::vector<std::string> Paths = Dir2.GetAllFile({ "jpg" });
		for (const std::string& FilePath : Paths)
		{
			Ext_DirectXTexture::LoadTexture(FilePath.c_str());
		}
	}

	// StoneWall 텍스쳐 로드
	{
		Base_Directory Dir;
		Dir.MakePath("../Resource/StaticMesh/Wall/Texture");
		std::vector<std::string> Paths = Dir.GetAllFile({ "jpg" });
		for (const std::string& FilePath : Paths)
		{
			Dir.SetPath(FilePath.c_str());
			std::string ExtensionName = Dir.GetExtension();
			std::string FileName = Dir.GetFileName();
			Ext_DirectXTexture::LoadTexture(FilePath.c_str());
		}
	}

	// 액터 생성
	{
		CreateActor<Character>("Character");
		CreateActor<TestPlatform>("TestPlatform");

		std::shared_ptr<CubeActor> CubeActor1 = CreateActor<CubeActor>("CubeActor1");
		CubeActor1->GetTransform()->SetLocalPosition({ -100.f, 100.f, 100.f });

		std::shared_ptr<CubeActor> CubeActor2 = CreateActor<CubeActor>("CubeActor2");
		CubeActor2->GetTransform()->SetLocalPosition({ -100.f, 180.f, 100.f });
		CubeActor2->SetRotate();

		std::shared_ptr<SphereActor> SphereActor1 = CreateActor<SphereActor>("SphereActor1");
		SphereActor1->GetTransform()->SetLocalPosition({ 0.f, 100.f, 100.f });

		std::shared_ptr<SphereActor> SphereActor2 = CreateActor<SphereActor>("SphereActor2");
		SphereActor2->GetTransform()->SetLocalPosition({ 0.f, 180.f, 100.f });
		SphereActor2->SetRotate();

		std::shared_ptr<CylinderActor> CylinderActor1 = CreateActor<CylinderActor>("CylinderActor1");
		CylinderActor1->GetTransform()->SetLocalPosition({ 100.f, 100.f, 100.f });

		std::shared_ptr<CylinderActor> CylinderActor2 = CreateActor<CylinderActor>("CylinderActor2");
		CylinderActor2->GetTransform()->SetLocalPosition({ 100.f, 180.f, 100.f });
		CylinderActor2->SetRotate();

		std::shared_ptr<StaticMeshActor> StaticMeshActor1 = CreateActor<StaticMeshActor>("StaticMeshActor1");
		StaticMeshActor1->GetTransform()->SetLocalPosition({ 200.f, 100.f, 100.f });

		std::shared_ptr<StaticMeshActor> StaticMeshActor2 = CreateActor<StaticMeshActor>("StaticMeshActor2");
		StaticMeshActor2->GetTransform()->SetLocalPosition({ 200.f, 180.f, 100.f });
		StaticMeshActor2->SetRotate();

		std::shared_ptr<RectActor> Manual = CreateActor<RectActor>("RectActor");
		Manual->GetTransform()->SetLocalPosition({ 0.f, 300.f, -500.f });
		Manual->GetTransform()->AddLocalRotation({ 0.f, 180.f, 0.f });

		std::shared_ptr<StoneWallActor> StoneWall = CreateActor<StoneWallActor>("StoneWallActor");
		StoneWall->GetTransform()->SetLocalPosition({ 350.f, 150.f, -100.f });
		StoneWall->GetTransform()->AddLocalRotation({ 0.f, 90.f, 0.f });
	}
	
}


void TestScene::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);
}

