#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

class CylinderActor : public Ext_Actor
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

	void SetRotate() { bIsRotation = true; }

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

private:
	float AccTime = 0;
	bool bIsRotation = false;
	std::shared_ptr<class Ext_MeshComponent> MeshComp;
	
};