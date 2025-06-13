#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

class SkyBoxActor : public Ext_Actor
{
public:
	// constrcuter destructer
	SkyBoxActor() {}
	~SkyBoxActor() {}

	// delete Function
	SkyBoxActor(const SkyBoxActor& _Other) = delete;
	SkyBoxActor(SkyBoxActor&& _Other) noexcept = delete;
	SkyBoxActor& operator=(const SkyBoxActor& _Other) = delete;
	SkyBoxActor& operator=(SkyBoxActor&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

private:
	std::shared_ptr<class Ext_MeshComponent> MeshComp;
	
};