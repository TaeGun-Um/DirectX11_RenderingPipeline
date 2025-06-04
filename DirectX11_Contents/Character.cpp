#include "PrecompileHeader.h"
#include "Character.h"

#include <DirectX11_Base/Base_Directory.h>
#include <DirectX11_Base/Base_Input.h>

#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Extension/Ext_DirectXMesh.h>
#include <DirectX11_Extension/Ext_DirectXTexture.h>
#include <DirectX11_Extension/Ext_DynamicMeshComponent.h>
#include <DirectX11_Extension/Ext_CollisionComponent.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Scene.h>
#include <DirectX11_Extension/Ext_Camera.h>

void Character::Start()
{
	CreateInput();
	CreateFSM();
	
	BodyMesh = CreateComponent<Ext_DynamicMeshComponent>("BodyMesh");
	BodyMesh->CreateMeshComponentUnit("Girl", "DynamicNonG");
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
	BodyCollisionMesh->CreateMeshComponentUnit("Sphere", "Debug");

	BodyCollisionMesh->GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
	BodyCollision->GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
	BodyCollisionMesh->GetTransform()->SetLocalPosition({ 0.f, 25.f, 0.f });
	BodyCollision->GetTransform()->SetLocalPosition({ 0.f, 25.f, 0.f });

	BodyMesh->GetTransform()->SetLocalPosition({ 0.f, 102.5f, 0.f }); // ���ǰ�

	GetTransform()->SetLocalPosition({0.f, 10.f, 0.f});

	//GetOwnerScene().lock()->GetMainCamera()->GetTransform()->SetParent(GetTransform());
	GetOwnerScene().lock()->GetMainCamera()->GetTransform()->SetLocalPosition({ 0.f, CameraHeight, -CameraDistance });
	GetOwnerScene().lock()->GetMainCamera()->GetTransform()->SetLocalRotation({ CamPitch, CamYaw, 0.f });

	GetTransform()->SetLocalPosition({ 0, 100, 0 });
	// GetTransform()->SetLocalScale({ 0.05f, 0.05f, 0.05f });
}

void Character::Update(float _DeltaTime)
{
	AccTime += _DeltaTime;

	if (!GetOwnerScene().lock()->GetMainCamera()->IsCameraAcc())
	{
		if (!BodyCollision->Collision(CollisionGroup::Platform))
		{
			GetTransform()->AddLocalPosition({ 0.f, -Gravity * _DeltaTime, 0.f });
		}
		else
		{
			bIsGround = true;
		}

		PlayerFSM.Update(_DeltaTime);

		{
			// 1) ������ �ڵ��� ���ϰ�(Ext_Camera�� Base_Windows����)
			HWND hWnd = Base_Windows::GetHWnd();

			// 2) Ŭ���̾�Ʈ ���� ũ�� ���ϱ�
			RECT rc;
			GetClientRect(hWnd, &rc);
			// Ŭ���̾�Ʈ ���� ���� ����� (0,0), ������ �ϴ��� (Width, Height)

			// 3) Ŭ���̾�Ʈ ������ �߾� ��ǥ (Ŭ���̾�Ʈ ���� ��ǥ��)
			POINT clientCenter =
			{
				(rc.right - rc.left) / 2,
				(rc.bottom - rc.top) / 2
			};

			// 4) �װ��� ����ũ�� ��ǥ�衱�� �ٲ����� (ClientToScreen)
			POINT screenCenter = clientCenter;
			ClientToScreen(hWnd, &screenCenter);
			// ���� screenCenter�� ���� ����� ��ü ȭ�� �󿡼� ������ �߾� ��ǥ

			// 5) ���� Ŀ���� ��ũ�� ��ǥ�� �о�´�
			POINT curMouse;
			GetCursorPos(&curMouse);

			// 6) �� ��� (��ũ�� ��ǥ ��������)
			int deltaX = curMouse.x - screenCenter.x;
			int deltaY = curMouse.y - screenCenter.y;

			// 7) �İ� 0�� �ƴ� ���� ȸ������ �ݿ�
			//    (�İ� 0�̶�� ���콺�� �������� ���� �Ŵϱ� ȸ���� 0)
			CamYaw += deltaX * MouseSensitivity;
			CamPitch -= deltaY * MouseSensitivity;

			CamPitch = std::clamp(CamPitch, 15.0f, 15.0f);

			// 9) Ŀ���� �ٽ� ������ �߾�(��ũ�� ��ǥ)���� ����
			SetCursorPos(screenCenter.x, screenCenter.y);

			// 10) ���� ȸ���� ���� (X=Pitch, Y=Yaw)
			GetOwnerScene().lock()->GetMainCamera()->GetTransform()->SetLocalRotation({ CamPitch, CamYaw, 0.f });
		}

		// ������������������������������������������������������������������������������������������������������������������������������������������������������
		// [ī�޶� ��ġ ����] : Character ���� ��ġ ��, ���� Up/Forward ��� �����¸�ŭ �̵�
		// ������������������������������������������������������������������������������������������������������������������������������������������������������
		{
			float4 charWorldPos = GetTransform()->GetWorldPosition();
			float4 camLocalUp = GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalUpVector();
			float4 camLocalFwd = GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalForwardVector();

			float4 desiredCamPos =
				charWorldPos
				+ camLocalUp * CameraHeight
				- camLocalFwd * CameraDistance;

			GetOwnerScene().lock()->GetMainCamera()->GetTransform()->SetLocalPosition(desiredCamPos);
		}
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
		.StateValue = Character_FSM::Idle, // ������ ���ֱ�
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

			if (Base_Input::IsDown("Up") && true == bIsGround)
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
		.StateValue = Character_FSM::Walking, // ������ �ȱ�
		.Start = [=]
		{
			BodyMesh->SetAnimation("Walking", true);
		},
		.Update = [=](float _DeltaTime)
		{
			float4 CamForward = GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalForwardVector();
			CamForward.y = 0.f;
			CamForward.Normalize();

			float4 MoveDelta = float4::ZERO;
			MoveDelta += CamForward * MoveSpeed * _DeltaTime;
			GetTransform()->AddLocalPosition(MoveDelta);

			GetTransform()->SetLocalRotation({ 0.f, GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalRotation().y, 0.f });

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

			if (Base_Input::IsDown("Up") && true == bIsGround)
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
		.StateValue = Character_FSM::WalkingLeft, // �������� �ȱ�
		.Start = [=]
		{
			BodyMesh->SetAnimation("WalkingLeft", true);
		},
		.Update = [=](float _DeltaTime)
		{
			float4 CamRight = GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalRightVector();
			CamRight.y = 0.f;
			CamRight.Normalize();

			float4 MoveDelta = float4::ZERO;
			MoveDelta += CamRight * MoveSpeed * _DeltaTime;
			GetTransform()->AddLocalPosition(-MoveDelta);

			GetTransform()->SetLocalRotation({ 0.f, GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalRotation().y, 0.f });

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

			if (Base_Input::IsDown("Up") && true == bIsGround)
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
		.StateValue = Character_FSM::WalkingRight, // ���������� �ȱ�
		.Start = [=]
		{
			BodyMesh->SetAnimation("WalkingRight", true);
		},
		.Update = [=](float _DeltaTime)
		{
			float4 CamRight = GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalRightVector();
			CamRight.y = 0.f;
			CamRight.Normalize();

			float4 MoveDelta = float4::ZERO;
			MoveDelta += CamRight * MoveSpeed * _DeltaTime;
			GetTransform()->AddLocalPosition(MoveDelta);

			GetTransform()->SetLocalRotation({ 0.f, GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalRotation().y, 0.f });

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

			if (Base_Input::IsDown("Up") && true == bIsGround)
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
		.StateValue = Character_FSM::WalkingBackward, // �ڷ� �ȱ�
		.Start = [=]
		{
			BodyMesh->SetAnimation("WalkingBackward", true);
		},
		.Update = [=](float _DeltaTime)
		{
			float4 CamForward = GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalForwardVector();
			CamForward.y = 0.f;
			CamForward.Normalize();

			float4 MoveDelta = float4::ZERO;
			MoveDelta += CamForward * MoveSpeed * _DeltaTime;
			GetTransform()->AddLocalPosition(-MoveDelta);

			GetTransform()->SetLocalRotation({ 0.f, GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalRotation().y, 0.f });

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

			if (Base_Input::IsDown("Up") && true == bIsGround)
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
		.StateValue = Character_FSM::Jump, // ������ ����
		.Start = [=]
		{
			bIsGround = false;
			bIsJump = true;
			VerticalVelocity = JumpStrength;
			BodyMesh->SetAnimation("JumpFront");
		},
		.Update = [=](float _DeltaTime)
		{
			float4 CamForward = GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalForwardVector();
			CamForward.y = 0.f;
			CamForward.Normalize();

			float4 MoveDelta = float4::ZERO;
			MoveDelta += CamForward * MoveSpeed * _DeltaTime;
			GetTransform()->AddLocalPosition(MoveDelta);

			GetTransform()->SetLocalRotation({ 0.f, GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalRotation().y, 0.f });


			VerticalVelocity -= Gravity * _DeltaTime;
			float DeltaY = VerticalVelocity * _DeltaTime;
			GetTransform()->AddLocalPosition({ 0.f, DeltaY, 0.f });
			JumpAccTime += _DeltaTime;

			// 4) ���� ���� üũ
			if (BodyCollision->Collision(CollisionGroup::Platform) && JumpAccTime > 0.5f)
			{
				// ���鿡 ������Ƿ� ���� ����(Idle �̵�)
				GetTransform()->AddLocalPosition({ 0.f, /*���� ���� �ʿ� ��*/ 0.f, 0.f });
				PlayerFSM.ChangeState(Character_FSM::Idle);
				return;
			}
		},
		.End = [=]
		{
			VerticalVelocity = 0.0f;
			JumpAccTime = 0.f;
			bIsJump = false;
		}
		}
	);

	PlayerFSM.CreateState({
		.StateValue = Character_FSM::JumpBackward, // �ڷ� ����
		.Start = [=]
		{
			bIsGround = false;
			bIsJump = true;
			VerticalVelocity = JumpStrength;
			BodyMesh->SetAnimation("JumpBackward");
		},
		.Update = [=](float _DeltaTime)
		{
			float4 CamForward = GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalForwardVector();
			CamForward.y = 0.f;
			CamForward.Normalize();

			float4 MoveDelta = float4::ZERO;
			MoveDelta += CamForward * MoveSpeed * _DeltaTime;
			GetTransform()->AddLocalPosition(-MoveDelta);

			GetTransform()->SetLocalRotation({ 0.f, GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalRotation().y, 0.f });

			VerticalVelocity -= Gravity * _DeltaTime;
			float DeltaY = VerticalVelocity * _DeltaTime;
			GetTransform()->AddLocalPosition({ 0.f, DeltaY, 0.f });
			JumpAccTime += _DeltaTime;

			// 4) ���� ���� üũ
			if (BodyCollision->Collision(CollisionGroup::Platform) && JumpAccTime > 0.5f)
			{
				// ���鿡 ������Ƿ� ���� ����(Idle �̵�)
				GetTransform()->AddLocalPosition({ 0.f, /*���� ���� �ʿ� ��*/ 0.f, 0.f });
				PlayerFSM.ChangeState(Character_FSM::Idle);
				return;
			}
		},
		.End = [=]
		{
			VerticalVelocity = 0.0f;
			JumpAccTime = 0.f;
			bIsJump = false;
		}
		}
	);

	PlayerFSM.CreateState({
		.StateValue = Character_FSM::Attack,
		.Start = [=]
		{
			GetTransform()->SetLocalRotation({ 0.f, GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalRotation().y, 0.f });
			BodyMesh->SetAnimation("Attack");
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
