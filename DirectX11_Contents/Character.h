#pragma once
#include <DirectX11_Extension/Ext_Actor.h>
#include <DirectX11_Extension/Ext_FSM.h>

enum Character_FSM
{
	Idle,
	Walking,
	WalkingLeft,
	WalkingRight,
	WalkingBackward,
	Jump,
	JumpBackward,
	Attack
};

class Character : public Ext_Actor
{
public:
	// constrcuter destructer
	Character() {}
	~Character() {}

	// delete Function
	Character(const Character& _Other) = delete;
	Character(Character&& _Other) noexcept = delete;
	Character& operator=(const Character& _Other) = delete;
	Character& operator=(Character&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;
	
private:
	void CreateInput();
	void CreateFSM();

	Ext_FSM PlayerFSM;

	std::shared_ptr<class Ext_DynamicMeshComponent> BodyMesh;
	std::shared_ptr<class Ext_CollisionComponent> BodyCollision;
	std::shared_ptr<class Ext_MeshComponent> BodyCollisionMesh;
	
	// 카메라
	std::shared_ptr<class Ext_Camera> PlayerCamera;
	float CameraDistance = 130.0f;
	float CameraHeight = 100.0f;

	// 카메라 회전용 각도 (라디안)
	float CamYaw = 0.0f;
	float CamPitch = 17.0f * DirectX::XM_PI / 180.0f; // 약간 아래를 보도록 15°
	float MouseSensitivity = 0.1f;

	float AccTime = 0.f;
	float MoveSpeed = 150.f;
	
	// 점프
	float JumpAccTime = 0.f;
	bool bIsGround = false;
	bool bIsJump = false;
	float VerticalVelocity = 0.0f;     // 수직 속도 (단위: 월드 유닛/초)
	float JumpStrength = 220.0f;   // 점프 시작 시 위쪽으로 부여할 초기 속도
	float Gravity = 150.0f;  // 중력 가속도 (단위: 월드 유닛/초^2)
};