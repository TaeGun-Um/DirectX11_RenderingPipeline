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

	BodyMesh->GetTransform()->SetLocalPosition({ 0.f, 102.5f, 0.f }); // 임의값

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
			// 1) 윈도우 핸들을 구하고(Ext_Camera나 Base_Windows에서)
			HWND hWnd = Base_Windows::GetHWnd();

			// 2) 클라이언트 영역 크기 구하기
			RECT rc;
			GetClientRect(hWnd, &rc);
			// 클라이언트 영역 왼쪽 상단이 (0,0), 오른쪽 하단이 (Width, Height)

			// 3) 클라이언트 영역의 중앙 좌표 (클라이언트 내부 좌표계)
			POINT clientCenter =
			{
				(rc.right - rc.left) / 2,
				(rc.bottom - rc.top) / 2
			};

			// 4) 그것을 “스크린 좌표계”로 바꿔주자 (ClientToScreen)
			POINT screenCenter = clientCenter;
			ClientToScreen(hWnd, &screenCenter);
			// 이제 screenCenter는 실제 모니터 전체 화면 상에서 윈도우 중앙 좌표

			// 5) 현재 커서를 스크린 좌표로 읽어온다
			POINT curMouse;
			GetCursorPos(&curMouse);

			// 6) Δ 계산 (스크린 좌표 기준으로)
			int deltaX = curMouse.x - screenCenter.x;
			int deltaY = curMouse.y - screenCenter.y;

			// 7) Δ가 0이 아닐 때만 회전값에 반영
			//    (Δ가 0이라면 마우스를 움직이지 않은 거니까 회전도 0)
			CamYaw += deltaX * MouseSensitivity;
			CamPitch -= deltaY * MouseSensitivity;

			CamPitch = std::clamp(CamPitch, 15.0f, 15.0f);

			// 9) 커서를 다시 윈도우 중앙(스크린 좌표)으로 고정
			SetCursorPos(screenCenter.x, screenCenter.y);

			// 10) 로컬 회전에 적용 (X=Pitch, Y=Yaw)
			GetOwnerScene().lock()->GetMainCamera()->GetTransform()->SetLocalRotation({ CamPitch, CamYaw, 0.f });
		}

		// ───────────────────────────────────────────────────────────────────────────
		// [카메라 위치 갱신] : Character 월드 위치 얻어서, 로컬 Up/Forward 기반 오프셋만큼 이동
		// ───────────────────────────────────────────────────────────────────────────
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
		.StateValue = Character_FSM::Walking, // 앞으로 걷기
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
		.StateValue = Character_FSM::WalkingLeft, // 왼쪽으로 걷기
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
		.StateValue = Character_FSM::WalkingRight, // 오른쪽으로 걷기
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
		.StateValue = Character_FSM::WalkingBackward, // 뒤로 걷기
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
		.StateValue = Character_FSM::Jump, // 앞으로 점프
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

			// 4) 착지 여부 체크
			if (BodyCollision->Collision(CollisionGroup::Platform) && JumpAccTime > 0.5f)
			{
				// 지면에 닿았으므로 상태 종료(Idle 이동)
				GetTransform()->AddLocalPosition({ 0.f, /*착지 보정 필요 시*/ 0.f, 0.f });
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
		.StateValue = Character_FSM::JumpBackward, // 뒤로 점프
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

			// 4) 착지 여부 체크
			if (BodyCollision->Collision(CollisionGroup::Platform) && JumpAccTime > 0.5f)
			{
				// 지면에 닿았으므로 상태 종료(Idle 이동)
				GetTransform()->AddLocalPosition({ 0.f, /*착지 보정 필요 시*/ 0.f, 0.f });
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
