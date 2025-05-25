#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

class RotateFace : public Ext_Actor
{
public:
	// constrcuter destructer
	RotateFace();
	~RotateFace();

	// delete Function
	RotateFace(const RotateFace& _Other) = delete;
	RotateFace(RotateFace&& _Other) noexcept = delete;
	RotateFace& operator=(const RotateFace& _Other) = delete;
	RotateFace& operator=(RotateFace&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;
	void Destroy() override;
	
};