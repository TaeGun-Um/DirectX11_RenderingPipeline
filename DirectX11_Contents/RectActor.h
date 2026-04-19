#pragma once
#include "RotatingMeshActor.h"

class RectActor : public RotatingMeshActor
{
public:
	// constrcuter destructer
	RectActor() {}
	~RectActor() {}

	// delete Function
	RectActor(const RectActor& _Other) = delete;
	RectActor(RectActor&& _Other) noexcept = delete;
	RectActor& operator=(const RectActor& _Other) = delete;
	RectActor& operator=(RectActor&& _Other) noexcept = delete;

protected:
	void Start() override;

private:

};
