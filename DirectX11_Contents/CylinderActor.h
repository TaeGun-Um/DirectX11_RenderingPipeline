#pragma once
#include "RotatingMeshActor.h"

class CylinderActor : public RotatingMeshActor
{
public:
	// constrcuter destructer
	CylinderActor() {}
	~CylinderActor() {}

	// delete Function
	CylinderActor(const CylinderActor& _Other) = delete;
	CylinderActor(CylinderActor&& _Other) noexcept = delete;
	CylinderActor& operator=(const CylinderActor& _Other) = delete;
	CylinderActor& operator=(CylinderActor&& _Other) noexcept = delete;

protected:
	void Start() override;

private:

};
