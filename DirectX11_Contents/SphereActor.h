#pragma once
#include "RotatingMeshActor.h"

class SphereActor : public RotatingMeshActor
{
public:
	// constrcuter destructer
	SphereActor() {}
	~SphereActor() {}

	// delete Function
	SphereActor(const SphereActor& _Other) = delete;
	SphereActor(SphereActor&& _Other) noexcept = delete;
	SphereActor& operator=(const SphereActor& _Other) = delete;
	SphereActor& operator=(SphereActor&& _Other) noexcept = delete;

protected:
	void Start() override;

private:

};
