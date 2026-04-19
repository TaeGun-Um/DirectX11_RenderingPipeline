#pragma once
#include "RotatingMeshActor.h"

class StaticMeshActor : public RotatingMeshActor
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

protected:
	void Start() override;

private:

};
