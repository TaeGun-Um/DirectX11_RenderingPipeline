#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

class StaticMeshActor : public Ext_Actor
{
public:
	// constrcuter destructer
	StaticMeshActor() {}
	~StaticMeshActor() {}

	// delete Function
	StaticMeshActor(const StaticMeshActor& _Other) = delete;
	StaticMeshActor(StaticMeshActor&& _Other) noexcept = delete;
	StaticMeshActor& operator=(const StaticMeshActor& _Other) = delete;
	StaticMeshActor& operator=(StaticMeshActor&& _Other) noexcept = delete;

	void SetRotate() { bIsRotation = true; }

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

private:
	float AccTime = 0;
	bool bIsRotation = false;
	std::shared_ptr<class Ext_MeshComponent> MeshComp;
	
};