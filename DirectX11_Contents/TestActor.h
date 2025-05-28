#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

class TestActor : public Ext_Actor
{
public:
	// constrcuter destructer
	TestActor() {}
	~TestActor() {}

	// delete Function
	TestActor(const TestActor& _Other) = delete;
	TestActor(TestActor&& _Other) noexcept = delete;
	TestActor& operator=(const TestActor& _Other) = delete;
	TestActor& operator=(TestActor&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;
	
private:
	std::shared_ptr<class Ext_MeshComponent> MeshComp;
	
};