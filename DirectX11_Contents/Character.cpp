#include "PrecompileHeader.h"
#include "Character.h"

#include <DirectX11_Base/Base_Directory.h>

#include <DirectX11_Extension/Ext_DirectXMesh.h>
#include <DirectX11_Extension/Ext_DirectXTexture.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_DynamicMeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

void Character::Start()
{
	GetTransform()->SetLocalPosition({ 0, 0, 5 });
	GetTransform()->SetLocalScale({ 0.05f, 0.05f, 0.05f });

	Base_Directory Dir1;
	Dir1.MakePath("../Resource/Mesh/Character/Mesh/Girl.fbx");
	Ext_DirectXMesh::CreateDynamicMesh(Dir1.GetPath());

	Base_Directory Dir2;
	Dir2.MakePath("../Resource/Mesh/Character/Texture");
	std::vector<std::string> Paths = Dir2.GetAllFile({ "png", "tga", "dss" });
	for (const std::string& FilePath : Paths)
	{
		Ext_DirectXTexture::LoadTexture(FilePath.c_str());
	}

	MeshComp = CreateComponent<Ext_DynamicMeshComponent>("BodyMesh");
	MeshComp->CreateMeshComponentUnit("Girl", "Animation");
	MeshComp->SetTexture("Ch03_1001_Diffuse.png", "BaseColor");

	Base_Directory Dir3;
	Dir3.MakePath("../Resource/Mesh/Character/Animation/Idle.fbx");
	MeshComp->CreateAnimation(Dir3.GetPath());

	//MeshComp2 = CreateComponent<Ext_MeshComponent>("BodyMesh");
	//MeshComp2->CreateMeshComponentUnit("Girl", "Basic");
	//MeshComp2->SetTexture("Ch03_1001_Diffuse.png", "BaseColor");
}

bool Switch = false;

void Character::Update(float _DeltaTime)
{
	AccTime += _DeltaTime;

	//if (AccTime > 3.f)
	//{
	//	AccTime = 0.f;
	//	Switch = !Switch;
	//	if (Switch)
	//	{
	//		MeshComp->SetAnimation(1);
	//	}
	//	else
	//	{
	//		MeshComp->SetAnimation(0);
	//	}
	//}

	__super::Update(_DeltaTime);
}