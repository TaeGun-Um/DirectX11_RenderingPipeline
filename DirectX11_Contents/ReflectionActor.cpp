#include "PrecompileHeader.h"
#include "ReflectionActor.h"

#include <DirectX11_Base/Base_Directory.h>

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>
#include <DirectX11_Extension/Ext_ReflectionComponent.h>

#include <DirectX11_Extension/Ext_DirectXTexture.h>

void ReflectionActor::Start()
{
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

	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Sphere", MaterialType::StaticCUBE);

	MeshComp->SetTexture("Gray.png");
	MeshComp->ShadowOn(ShadowType::Static);

	GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
}

void ReflectionActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);

	if (bIsRotation)
	{
		float MoveSpeed = 50.0f; // 초당 100 단위 회전
		MeshComp->GetTransform()->AddLocalRotation({ 0.f, MoveSpeed * _DeltaTime, 0.f });
	}
}

void ReflectionActor::SetReflection()
{
	Reflection = std::make_shared<Ext_ReflectionComponent>();
	static int n = 0;
	MeshComp->SetSampler("CubeMapSampler", "CubeMapSampler");
	Reflection->ReflectionInitialize(GetSharedFromThis<Ext_Actor>(), "TestReflection" + std::to_string(n++), float4(512, 512));
	MeshComp->SetTexture(Reflection->GetReflectionCubeTexture(), "ReflectionTexture");
}
