#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

class CubeActor : public Ext_Actor
{
public:
	// constrcuter destructer
	CubeActor();
	~CubeActor();

	// delete Function
	CubeActor(const CubeActor& _Other) = delete;
	CubeActor(CubeActor&& _Other) noexcept = delete;
	CubeActor& operator=(const CubeActor& _Other) = delete;
	CubeActor& operator=(CubeActor&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;
	void Destroy() override;
	
private:
	float AccTime = 0;
	
};