#pragma once
#include "Ext_Actor.h"

// 렌더링을 담당하는 Actor
class Ext_Camera : public Ext_Actor
{
	friend class Ext_Scene;

public:
	// constrcuter destructer
	Ext_Camera();
	~Ext_Camera();

	// delete Function
	Ext_Camera(const Ext_Camera& _Other) = delete;
	Ext_Camera(Ext_Camera&& _Other) noexcept = delete;
	Ext_Camera& operator=(const Ext_Camera& _Other) = delete;
	Ext_Camera& operator=(Ext_Camera&& _Other) noexcept = delete;


	float4x4 GetViewMatrix();

protected:
	void Start() override;
	void Update(float _DeltaTime) override {}
	void Destroy() override {}
	
private:
	void CameraTransformUpdate();

	D3D11_VIEWPORT ViewPortData;
	CameraType ProjectionType = CameraType::Perspective;
	float4x4 View;
	float4x4 Projection;
	float4x4 ViewPort;
	float FOV = 90.0f;
	float Near = 10.f;
	float Far = 100000.0f;
	float Width = 0.0f;
	float Height = 0.0f;
};