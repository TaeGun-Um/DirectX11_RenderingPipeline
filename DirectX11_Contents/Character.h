#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

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
	std::shared_ptr<class Ext_FBXMeshComponent> MeshComp;
	std::shared_ptr<class Ext_MeshComponent> MeshComp2;
	
};