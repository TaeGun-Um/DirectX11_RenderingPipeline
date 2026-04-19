#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

class StoneWallActor : public Ext_Actor
{
public:
	// constrcuter destructer
	StoneWallActor() {}
	~StoneWallActor() {}

	// delete Function
	StoneWallActor(const StoneWallActor& _Other) = delete;
	StoneWallActor(StoneWallActor&& _Other) noexcept = delete;
	StoneWallActor& operator=(const StoneWallActor& _Other) = delete;
	StoneWallActor& operator=(StoneWallActor&& _Other) noexcept = delete;

	void SetReflection();

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

private:
	std::shared_ptr<class Ext_CollisionComponent> WallBody;
	std::shared_ptr<class Ext_MeshComponent> WallBodyMesh;
	std::shared_ptr<class Ext_ReflectionComponent> Reflection;
};