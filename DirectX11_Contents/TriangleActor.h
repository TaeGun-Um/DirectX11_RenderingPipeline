#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

class TriangleActor : public Ext_Actor
{
public:
	// constrcuter destructer
	TriangleActor();
	~TriangleActor();

	// delete Function
	TriangleActor(const TriangleActor& _Other) = delete;
	TriangleActor(TriangleActor&& _Other) noexcept = delete;
	TriangleActor& operator=(const TriangleActor& _Other) = delete;
	TriangleActor& operator=(TriangleActor&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;
	void Destroy() override;
	
private:
	
};