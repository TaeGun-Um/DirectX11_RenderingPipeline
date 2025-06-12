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
	
	BodyMesh = CreateComponent<Ext_DynamicMeshComponent>("CharacterBodyMesh");
	BodyMesh->CreateMeshComponentUnit("Girl", MaterialType::Dynamic);
	BodyMesh->SetTexture("Ch03_1001_Diffuse.png", TextureType::BaseColor);
	//BodyMesh->ShadowOn(ShadowType::Dynamic);
	
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
	BodyCollision->SetCollsionType(CollsionType::Sphere3D);

	BodyCollisionMesh = CreateComponent<Ext_MeshComponent>("BodyCollisionMesh");
	BodyCollisionMesh->CreateMeshComponentUnit("Sphere", MaterialType::Debug);

	BodyCollisionMesh->GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
	BodyCollision->GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
	BodyCollisionMesh->GetTransform()->SetLocalPosition({ 0.f, 25.f, 0.f });
	BodyCollision->GetTransform()->SetLocalPosition({ 0.f, 25.f, 0.f });
	BodyCollisionMesh->UpdateOff();

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

	// ���� ī�޶� ���� ��尡 �ƴ� ����(��, ���� ����� ����) �� �ڵ带 ����
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

		// -------------------------------
		// F4 ������ ���콺 ��Ŀ�� ���
		// -------------------------------
		if (Base_Input::IsDown("Escape"))
		{
			GetOwnerScene().lock()->GetMainCamera()->bIsEscapeSwitch();
			if (GetOwnerScene().lock()->GetMainCamera()->IsEscape())
			{
				// ȭ�� �߾� �� ��ũ�� ��ǥ�� ��ȯ �� Ŀ�� ����
				HWND hWnd = Base_Windows::GetHWnd();
				RECT rc;
				GetClientRect(hWnd, &rc);
				POINT clientCenter = {
					(rc.right - rc.left) / 2,
					(rc.bottom - rc.top) / 2
				};
				POINT screenCenter = clientCenter;
				ClientToScreen(hWnd, &screenCenter);

				SetCursorPos(screenCenter.x, screenCenter.y);
				ShowCursor(FALSE);
			}
			else
			{
				ShowCursor(TRUE);
			}
		}

		// ---------------------------------------------
		// ī�޶� ���� ����̸鼭 �����콺 ĸó �ߡ��� ���� ���
		// ---------------------------------------------
		if (GetOwnerScene().lock()->GetMainCamera()->IsEscape())
		{
			// (A) ������ �ڵ� ��������
			HWND hWnd = Base_Windows::GetHWnd();

			// (B) Ŭ���̾�Ʈ ���� ũ�� ���ϱ�
			RECT Rc;
			GetClientRect(hWnd, &Rc);

			POINT ClientCenter = { (Rc.right - Rc.left) / 2, (Rc.bottom - Rc.top) / 2 };
			POINT ScreenCenter = ClientCenter;
			ClientToScreen(hWnd, &ScreenCenter);

			// (C) ���� Ŀ�� ��ġ �б�
			POINT curMouse;
			GetCursorPos(&curMouse);

			// (D) �� ���
			int deltaX = curMouse.x - ScreenCenter.x;
			int deltaY = curMouse.y - ScreenCenter.y;

			// (E) �ĸ� ī�޶� ȸ���� �ݿ�
			CamYaw += deltaX * MouseSensitivity;
			CamPitch -= deltaY * MouseSensitivity;
			CamPitch = std::clamp(CamPitch, 15.0f, 15.0f); // ��: ���� ȸ�� ����

			// (F) Ŀ���� �ٽ� ������ �߾�(��ũ�� ��ǥ)���� ����
			SetCursorPos(ScreenCenter.x, ScreenCenter.y);

			// (G) ī�޶� ȸ�� ���� (X=Pitch, Y=Yaw)
			GetOwnerScene().lock()->GetMainCamera()->GetTransform()->SetLocalRotation({ CamPitch, CamYaw, 0.f });
		}
		// ���� bMouseCaptured == false��� �� ��� ���� Ŀ�� ��� ���� �����Ƿ�, 
		// ����ڴ� �����Ӱ� Ŀ���� ������ �� �ְ�, ī�޶� ȸ������ ����

		// ------------------------------------------------------------
		// ī�޶� ��ġ ���� : Character ���� ��ġ ������� �׻� ����ٴ�(bMouseCaptured ���ο� ������� �׻� ī�޶� ��ġ�� ������Ʈ)
		// ------------------------------------------------------------
		{
			float4 CharWorldPosition = GetTransform()->GetWorldPosition();
			float4 CamLocalUp = GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalUpVector();
			float4 CamLocalForward = GetOwnerScene().lock()->GetMainCamera()->GetTransform()->GetLocalForwardVector();
			float4 CamPososition = CharWorldPosition + CamLocalUp * CameraHeight - CamLocalForward * CameraDistance;

			GetOwnerScene().lock()->GetMainCamera()	->GetTransform()->SetLocalPosition(CamPososition);
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
