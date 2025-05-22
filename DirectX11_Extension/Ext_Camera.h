#pragma once
#include "Ext_Actor.h"

// 화면의 렌더링을 담당하는 Actor
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

	float4x4 GetViewMatrix() { return ViewMatrix; }
	float4x4 GetProjectionMatrix() { return ProjectionMatrix; }
	float4x4 GetViewPortMatrix() { return ViewPortMatrix; }

	void PushMeshComponent(std::shared_ptr<class Ext_MeshComponent> _MeshComponent);

protected:
	void Start() override;
	void Update(float _DeltaTime) override {}
	void Destroy() override {}
	
private:
	void CameraTransformUpdate();

	std::map<int, std::vector<std::shared_ptr<class Ext_MeshComponent>>> MeshComponents;

	D3D11_VIEWPORT ViewPortData;
	CameraType ProjectionType = CameraType::Perspective;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float4x4 ViewPortMatrix;
	float FOV = 90.0f;
	float Near = 10.f;
	float Far = 100000.0f;
	float Width = 0.0f;
	float Height = 0.0f;
};