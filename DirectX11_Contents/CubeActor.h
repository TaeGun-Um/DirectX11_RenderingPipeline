#pragma once
#include "RotatingMeshActor.h"

class CubeActor : public RotatingMeshActor
{
public:
	// constrcuter destructer
	CubeActor() {}
	~CubeActor() {}

	// delete Function
	CubeActor(const CubeActor& _Other) = delete;
	CubeActor(CubeActor&& _Other) noexcept = delete;
	CubeActor& operator=(const CubeActor& _Other) = delete;
	CubeActor& operator=(CubeActor&& _Other) noexcept = delete;

protected:
	void Start() override;

private:

};
