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
#include <DirectX11_Extension/Ext_SpringArm.h>

void Character::Start()
{
	CreateInput();
	CreateFSM();
	
	BodyMesh = CreateComponent<Ext_DynamicMeshComponent>("CharacterBodyMesh");
	BodyMesh->CreateMeshComponentUnit("Girl", MaterialType::Dynamic);
	BodyMesh->SetTexture("Ch03_1001_Diffuse.png", TextureType::BaseColor);
	BodyMesh->ShadowOn(ShadowType::Dynamic);
	
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

	BodyMesh->GetTransform()->SetLocalPosition({ 0.f, 102.5f, 0.f }); // 임의값

	GetTransform()->SetLocalPosition({0.f, 10.f, 0.f});

	// SpringArm attached to character; drives camera pos/rot
	CameraArm = CreateComponent<Ext_SpringArm>("CameraArm");
	CameraArm->SetTargetArmLength(CameraDistance);
	CameraArm->SetTargetOffset({ 0.f, CameraHeight, 0.f, 0.f });
	CameraArm->SetEnableLag(true);
	CameraArm->SetCameraLagSpeed(15.0f);
	CameraArm->SetRotationLagSpeed(20.0f);
	CameraArm->SetPitchClamp(-70.0f, 70.0f);
	CameraArm->SetRotation(17.0f, 0.0f);

	GetTransform()->SetLocalPosition({ 0, 100, 0 });
}

void Character::Update(float _DeltaTime)
{
	AccTime += _DeltaTime;

	// 메인 카메라가 자유 모드가 아닐 때만(즉, 추적 모드일 때만) 이 코드를 실행
	auto MainCam = GetOwnerScene().lock()->GetMainCamera();
	if (!MainCam->IsCameraAcc())
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

		// F4: toggle mouse capture
		if (Base_Input::IsDown("Escape"))
		{
			MainCam->bIsEscapeSwitch();
			Base_Input::SetCursorLocked(MainCam->IsEscape());
		}

		// When captured, feed mouse delta to SpringArm rotation
		if (MainCam->IsEscape())
		{
			if (!Base_Input::IsCursorLocked()) Base_Input::SetCursorLocked(true);
			float DeltaX = static_cast<float>(Base_Input::GetMouseDeltaX());
			float DeltaY = static_cast<float>(Base_Input::GetMouseDeltaY());
			CameraArm->AddRotationInput(DeltaY * MouseSensitivity, DeltaX * MouseSensitivity);
		}
		else if (Base_Input::IsCursorLocked())
		{
			Base_Input::SetCursorLocked(false);
		}

		// Apply SpringArm socket pose to main camera
		const float4& ArmPos = CameraArm->GetSocketWorldPosition();
		const float4& ArmRot = CameraArm->GetSocketWorldRotation();
		MainCam->GetTransform()->SetLocalPosition(ArmPos);
		MainCam->GetTransform()->SetLocalRotation(ArmRot);
	}

		__super::Update(_DeltaTime);
}

void Character::CreateInput()
{
	// Movement/camera keys are registered globally in Ext_Core::Start
	Base_Input::CreateKey("Attack", VK_LBUTTON);
}

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
