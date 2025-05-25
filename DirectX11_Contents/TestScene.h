#pragma once
#include <DirectX11_Extension/Ext_Scene.h>

class TestScene : public Ext_Scene
{
public:
	// constrcuter destructer
	TestScene();
	~TestScene();

	// delete Function
	TestScene(const TestScene& _Other) = delete;
	TestScene(TestScene&& _Other) noexcept = delete;
	TestScene& operator=(const TestScene& _Other) = delete;
	TestScene& operator=(TestScene&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;
	void Destroy() override;
	
private:
	
};