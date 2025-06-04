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
	
	// ī�޶�
	std::shared_ptr<class Ext_Camera> PlayerCamera;
	float CameraDistance = 130.0f;
	float CameraHeight = 100.0f;

	// ī�޶� ȸ���� ���� (����)
	float CamYaw = 0.0f;
	float CamPitch = 17.0f * DirectX::XM_PI / 180.0f; // �ణ �Ʒ��� ������ 15��
	float MouseSensitivity = 0.1f;

	float AccTime = 0.f;
	float MoveSpeed = 150.f;
	
	// ����
	float JumpAccTime = 0.f;
	bool bIsGround = false;
	bool bIsJump = false;
	float VerticalVelocity = 0.0f;     // ���� �ӵ� (����: ���� ����/��)
	float JumpStrength = 220.0f;   // ���� ���� �� �������� �ο��� �ʱ� �ӵ�
	float Gravity = 150.0f;  // �߷� ���ӵ� (����: ���� ����/��^2)
};