#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

class BoxActor : public Ext_Actor
{
public:
	// constrcuter destructer
	BoxActor() {}
	~BoxActor() {}

	// delete Function
	BoxActor(const BoxActor& _Other) = delete;
	BoxActor(BoxActor&& _Other) noexcept = delete;
	BoxActor& operator=(const BoxActor& _Other) = delete;
	BoxActor& operator=(BoxActor&& _Other) noexcept = delete;

	void SetRotate() { bIsRotation = true; }

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

private:
	float AccTime = 0;
	bool bIsRotation = false;
	std::shared_ptr<class Ext_MeshComponent> MeshComp;
	
};