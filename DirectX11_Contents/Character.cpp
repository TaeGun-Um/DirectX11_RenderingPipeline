#include "PrecompileHeader.h"
#include "Character.h"

#include <DirectX11_Base/Base_Directory.h>
#include <DirectX11_Base/Base_Input.h>

#include <DirectX11_Extension/Ext_DirectXMesh.h>
#include <DirectX11_Extension/Ext_DirectXTexture.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_DynamicMeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

void Character::Start()
{
	CreateInput();
	CreateFSM();

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

	BodyMesh = CreateComponent<Ext_DynamicMeshComponent>("BodyMesh");
	BodyMesh->CreateMeshComponentUnit("Girl", "Animation");
	BodyMesh->SetTexture("Ch03_1001_Diffuse.png", "BaseColor");

	Base_Directory Dir3;
	Dir3.MakePath("../Resource/Mesh/Character/Animation");
	std::vector<std::string> Pathse = Dir3.GetAllFile({ "fbx" });
	for (const std::string& FilePath : Pathse)
	{
		BodyMesh->CreateAnimation(FilePath);
	}

	BodyMesh->SetAnimation("Idle", true);
	PlayerFSM.ChangeState(Character_FSM::Idle);
}

void Character::Update(float _DeltaTime)
{
	AccTime += _DeltaTime;
	PlayerFSM.Update(_DeltaTime);

	__super::Update(_DeltaTime);
}

void Character::CreateInput()
{
	//Base_Input::CreateKey("OnOff", 'Q');
	//Base_Input::CreateKey("Forword", 'W');
	//Base_Input::CreateKey("Back", 'S');
	//Base_Input::CreateKey("Right", 'D');
	//Base_Input::CreateKey("Left", 'A');
	//Base_Input::CreateKey("Up", VK_SPACE);
}

void Character::CreateFSM()
{
	PlayerFSM.CreateState({
		.StateValue = Character_FSM::Idle,
		.Start = [=] 
		{
			BodyMesh->SetAnimation("Idle", true);
		},
		.Update = [=](float _DeltaTime)
		{
			if (Base_Input::IsDown("Forword"))
			{
				PlayerFSM.ChangeState(Character_FSM::Walking);
				return;
			}
		},
		.End = [=]
		{

		}
		}
	);

	PlayerFSM.CreateState({
		.StateValue = Character_FSM::Walking,
		.Start = [=]
		{
			BodyMesh->SetAnimation("Walking", true);
		},
		.Update = [=](float _DeltaTime)
		{
			if (Base_Input::IsFree("Forword"))
			{
				PlayerFSM.ChangeState(Character_FSM::Idle);
				return;
			}

			if (Base_Input::IsDown("Up"))
			{
				PlayerFSM.ChangeState(Character_FSM::Jump);
				return;
			}
		},
		.End = [=]
		{

		}
		}
	);

	PlayerFSM.CreateState({
		.StateValue = Character_FSM::Jump,
		.Start = [=]
		{
			IsJump = true;
			BodyMesh->SetAnimation("Jump", true);
		},
		.Update = [=](float _DeltaTime)
		{
			if (true == BodyMesh->IsAnimationEnd())
			{
				PlayerFSM.ChangeState(Character_FSM::Idle);
				return;
			}
		},
		.End = [=]
		{
			IsJump = false;
		}
		}
	);

	PlayerFSM.CreateState({
		.StateValue = Character_FSM::Attack,
		.Start = [=]
		{

		},
		.Update = [=](float _DeltaTime)
		{

		},
		.End = [=]
		{

		}
		}
	);
}
