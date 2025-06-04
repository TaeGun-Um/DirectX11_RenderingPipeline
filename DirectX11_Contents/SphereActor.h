#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

class SphereActor : public Ext_Actor
{
public:
	// constrcuter destructer
	SphereActor() {}
	~SphereActor() {}

	// delete Function
	SphereActor(const SphereActor& _Other) = delete;
	SphereActor(SphereActor&& _Other) noexcept = delete;
	SphereActor& operator=(const SphereActor& _Other) = delete;
	SphereActor& operator=(SphereActor&& _Other) noexcept = delete;

	void SetRotate() { bIsRotation = true; }

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

private:
	float AccTime = 0;
	bool bIsRotation = false;
	std::shared_ptr<class Ext_MeshComponent> MeshComp;
	
};