#include "PrecompileHeader.h"
#include "Character.h"

#include <DirectX11_Base/Base_Directory.h>
#include <DirectX11_Base/Base_Input.h>

#include <DirectX11_Extension/Ext_DirectXMesh.h>
#include <DirectX11_Extension/Ext_DirectXTexture.h>
#include <DirectX11_Extension/Ext_DynamicMeshComponent.h>
#include <DirectX11_Extension/Ext_CollisionComponent.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

void Character::Start()
{
	CreateInput();
	CreateFSM();

	GetTransform()->SetLocalPosition({ 0, 0, 5 });
	GetTransform()->SetLocalScale({ 0.05f, 0.05f, 0.05f });

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
	
	BodyMesh = CreateComponent<Ext_DynamicMeshComponent>("BodyMesh");
	BodyMesh->CreateMeshComponentUnit("Girl", "Animation");
	BodyMesh->SetTexture("Ch03_1001_Diffuse.png", "BaseColor");
	
	Base_Directory Dir3;
	Dir3.MakePath("../Resource/Character/Animation");
	std::vector<std::string> Pathse = Dir3.GetAllFile({ "fbx" });
	for (const std::string& FilePath : Pathse)
	{
		BodyMesh->CreateAnimation(FilePath);
	}
	
	BodyMesh->SetAnimation("Idle", true);
	PlayerFSM.ChangeState(Character_FSM::Idle);

	BodyCollision = CreateComponent<Ext_CollisionComponent>("BodyCollision", static_cast<int>(CollisionGroup::Player));
	BodyCollision->SetCollsionType(CollsionType::AABB3D);

	BodyCollisionMesh = CreateComponent<Ext_MeshComponent>("BodyCollisionMesh");
	BodyCollisionMesh->CreateMeshComponentUnit("Box", "Basic");

	BodyCollisionMesh->GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
	BodyCollision->GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });

	GetTransform()->SetLocalPosition({0.f, 30.f, 0.f});
}

void Character::Update(float _DeltaTime)
{
	AccTime += _DeltaTime;
	PlayerFSM.Update(_DeltaTime);

	float MoveSpeed = 5.0f;
	if (!BodyCollision->Collision(CollisionGroup::Platform))
	{
		GetTransform()->AddLocalPosition({ 0.f, -MoveSpeed * _DeltaTime, 0.f });
	}
	else
	{
		// todo
		int a = 0;
	}

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
	Base_Input::CreateKey("Attack", VK_LBUTTON);
}

// Idle,
// Walking,
// WalkingLeft,
// WalkingRight,
// WalkingBackward,
// Jump,
// JumpFront,
// JumpBackward,
// Attack

void Character::CreateFSM()
{
	PlayerFSM.CreateState({
		.StateValue = Character_FSM::Idle, // 가만히 서있기
		.Start = [=] 
		{
			BodyMesh->SetAnimation("Idle", true);
		},
		.Update = [=](float _DeltaTime)
		{
			if (Base_Input::IsPress("Forword"))
			{
				PlayerFSM.ChangeState(Character_FSM::Walking);
				return;
			}
			else if (Base_Input::IsPress("Left"))
			{
				PlayerFSM.ChangeState(Character_FSM::WalkingLeft);
				return;
			}
			else if (Base_Input::IsPress("Right"))
			{
				PlayerFSM.ChangeState(Character_FSM::WalkingRight);
				return;
			}
			else if (Base_Input::IsPress("Back"))
			{
				PlayerFSM.ChangeState(Character_FSM::WalkingBackward);
				return;
			}

			if (Base_Input::IsDown("Attack"))
			{
				PlayerFSM.ChangeState(Character_FSM::Attack);
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
		.StateValue = Character_FSM::Walking, // 앞으로 걷기
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

			if (Base_Input::IsDown("Attack"))
			{
				PlayerFSM.ChangeState(Character_FSM::Attack);
				return;
			}

			if (Base_Input::IsDown("Up"))
			{
				PlayerFSM.ChangeState(Character_FSM::JumpFront);
				return;
			}
		},
		.End = [=]
		{

		}
		}
	);

	PlayerFSM.CreateState({
		.StateValue = Character_FSM::WalkingLeft, // 왼쪽으로 걷기
		.Start = [=]
		{
			BodyMesh->SetAnimation("WalkingLeft", true);
		},
		.Update = [=](float _DeltaTime)
		{
			if (Base_Input::IsFree("Left"))
			{
				PlayerFSM.ChangeState(Character_FSM::Idle);
				return;
			}

			if (Base_Input::IsDown("Attack"))
			{
				PlayerFSM.ChangeState(Character_FSM::Attack);
				return;
			}

			if (Base_Input::IsDown("Up"))
			{
				PlayerFSM.ChangeState(Character_FSM::JumpFront);
				return;
			}
		},
		.End = [=]
		{

		}
		}
	);

	PlayerFSM.CreateState({
		.StateValue = Character_FSM::WalkingRight, // 오른쪽으로 걷기
		.Start = [=]
		{
			BodyMesh->SetAnimation("WalkingRight", true);
		},
		.Update = [=](float _DeltaTime)
		{
			if (Base_Input::IsFree("Right"))
			{
				PlayerFSM.ChangeState(Character_FSM::Idle);
				return;
			}

			if (Base_Input::IsDown("Attack"))
			{
				PlayerFSM.ChangeState(Character_FSM::Attack);
				return;
			}

			if (Base_Input::IsDown("Up"))
			{
				PlayerFSM.ChangeState(Character_FSM::JumpFront);
				return;
			}
		},
		.End = [=]
		{

		}
		}
	);

	PlayerFSM.CreateState({
		.StateValue = Character_FSM::WalkingBackward, // 뒤로 걷기
		.Start = [=]
		{
			BodyMesh->SetAnimation("WalkingBackward", true);
		},
		.Update = [=](float _DeltaTime)
		{
			if (Base_Input::IsFree("Back"))
			{
				PlayerFSM.ChangeState(Character_FSM::Idle);
				return;
			}

			if (Base_Input::IsDown("Attack"))
			{
				PlayerFSM.ChangeState(Character_FSM::Attack);
				return;
			}

			if (Base_Input::IsDown("Up"))
			{
				PlayerFSM.ChangeState(Character_FSM::JumpBackward);
				return;
			}
		},
		.End = [=]
		{

		}
		}
	);

	PlayerFSM.CreateState({
		.StateValue = Character_FSM::Jump, // 제자리 점프
		.Start = [=]
		{
			IsJump = true;
			BodyMesh->SetAnimation("Jump");
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
		.StateValue = Character_FSM::JumpFront, // 앞으로 점프
		.Start = [=]
		{
			IsJump = true;
			BodyMesh->SetAnimation("JumpFront");
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
		.StateValue = Character_FSM::JumpBackward, // 뒤로 점프
		.Start = [=]
		{
			IsJump = true;
			BodyMesh->SetAnimation("JumpBackward");
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
			if (true == BodyMesh->IsAnimationEnd())
			{
				PlayerFSM.ChangeState(Character_FSM::Idle);
				return;
			}
		},
		.End = [=]
		{

		}
		}
	);
}
