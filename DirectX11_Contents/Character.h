#pragma once
#include <DirectX11_Extension/Ext_Actor.h>
#include <DirectX11_Extension/Ext_FSM.h>

enum Character_FSM
{
	Idle,
	Walking,
	Jump,
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
	std::shared_ptr<class Ext_MeshComponent> MeshComp2;
	
	bool IsJump = false;
	float AccTime = 0.f;
};