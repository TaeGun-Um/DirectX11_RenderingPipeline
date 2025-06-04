#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

class RectActor : public Ext_Actor
{
public:
	// constrcuter destructer
	RectActor() {}
	~RectActor() {}

	// delete Function
	RectActor(const RectActor& _Other) = delete;
	RectActor(RectActor&& _Other) noexcept = delete;
	RectActor& operator=(const RectActor& _Other) = delete;
	RectActor& operator=(RectActor&& _Other) noexcept = delete;

	void SetRotate() { bIsRotation = true; }

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

private:
	float AccTime = 0;
	bool bIsRotation = false;
	std::shared_ptr<class Ext_MeshComponent> MeshComp;

};