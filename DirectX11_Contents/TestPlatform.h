#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

class TestPlatform : public Ext_Actor
{
public:
	// constrcuter destructer
	TestPlatform() {}
	~TestPlatform() {}

	// delete Function
	TestPlatform(const TestPlatform& _Other) = delete;
	TestPlatform(TestPlatform&& _Other) noexcept = delete;
	TestPlatform& operator=(const TestPlatform& _Other) = delete;
	TestPlatform& operator=(TestPlatform&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DletaTime) override;
	
private:
	std::shared_ptr<class Ext_CollisionComponent> PlatformBody;
	std::shared_ptr<class Ext_MeshComponent> PlatformBodyMesh;
	
};