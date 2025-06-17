#include "PrecompileHeader.h"
#include "StoneWallActor.h"
#include <DirectX11_Extension/Ext_CollisionComponent.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

#include <DirectX11_Base/Base_Directory.h>
#include <DirectX11_Extension/Ext_ReflectionComponent.h>
#include <DirectX11_Extension/Ext_DirectXTexture.h>

void StoneWallActor::Start()
{
	//WallBody = CreateComponent<Ext_CollisionComponent>("WallBody", static_cast<int>(CollisionGroup::Wall));
	//WallBody->SetCollsionType(CollsionType::OBB3D);

	//WallBodyMesh = CreateComponent<Ext_MeshComponent>("WallBodyMesh");
	////WallBodyMesh->CreateMeshComponentUnit("Rect", MaterialType::PBR);
	////WallBodyMesh->SetTexture("StoneWall_BaseColor.jpg", TextureType::BaseColor);
	////WallBodyMesh->SetTexture("StoneWall_Normal.jpg", TextureType::Normal);
	////WallBodyMesh->ShadowOn();
	//WallBodyMesh->CreateMeshComponentUnit("Rect", MaterialType::Static);
	//WallBodyMesh->SetTexture("Gray.png");
	//WallBodyMesh->ShadowOn(ShadowType::Static);

	GetTransform()->SetLocalScale({ 300.f, 300.f, 1.f });

	// 큐브맵을 위한 텍스쳐 로드
	//{
	//	Base_Directory Dir;
	//	Dir.MakePath("../Resource/FX/ReflectionTexture");
	//	std::vector<std::string> Paths = Dir.GetAllFile({ "png" });

	//	std::vector<std::shared_ptr<Ext_DirectXTexture>> Texs;

	//	for (const std::string& FilePath : Paths)
	//	{
	//		Dir.SetPath(FilePath.c_str());
	//		std::string ExtensionName = Dir.GetExtension();
	//		std::string FileName = Dir.GetFileName();
	//		Texs.push_back(Ext_DirectXTexture::LoadTexture(FilePath.c_str()));
	//	}
	//}

	WallBodyMesh = CreateComponent<Ext_MeshComponent>("WallBodyMesh");
	WallBodyMesh->CreateMeshComponentUnit("Rect", MaterialType::Static);
	WallBodyMesh->SetTexture("Gray.png");
}

void StoneWallActor::Update(float _DletaTime)
{
	__super::Update(_DletaTime);
}

void StoneWallActor::SetReflection()
{
	Reflection = std::make_shared<Ext_ReflectionComponent>();
	static int n = 1;
	WallBodyMesh->SetSampler("CubeMapSampler", "CubeMapSampler");
	Reflection->ReflectionInitialize(GetSharedFromThis<Ext_Actor>(), "TestReflection" + std::to_string(n++), float4(512, 512));
	WallBodyMesh->SetTexture(Reflection->GetReflectionCubeTexture(), "ReflectionTexture");
}
