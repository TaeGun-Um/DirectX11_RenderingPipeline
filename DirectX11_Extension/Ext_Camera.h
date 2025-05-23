#pragma once
#include "Ext_Actor.h"

enum class SortType
{
	ZSort,
	YSort,
	UnKnown
};

enum class RenderPath
{
	Forward,
	Deferred,
	Alpha,
	Unknown
};

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

	const float4x4& GetViewMatrix() { return ViewMatrix; }
	const float4x4& GetProjectionMatrix() { return ProjectionMatrix; }
	const float4x4& GetViewPortMatrix() { return ViewPortMatrix; }

	void PushMeshComponent(std::shared_ptr<class Ext_MeshComponent> _MeshComponent);
	void PushMeshComponentUnit(std::shared_ptr<class Ext_MeshComponentUnit> _Unit, RenderPath _Path = RenderPath::Unknown);

protected:
	void Start() override;
	void Update(float _DeltaTime) override {}
	void Destroy() override {}
	
private:
	void CameraTransformUpdate();
	void MeshComponentTransformUpdate(std::shared_ptr<Ext_Camera> _Camera);
	void ViewPortSetting();
	void Rendering(float _Deltatime);

	std::map<int, std::vector<std::shared_ptr<class Ext_MeshComponent>>> MeshComponents; // 생성된 메시 컴포넌트들
	std::map<RenderPath, std::map<int, std::list<std::shared_ptr<class Ext_MeshComponentUnit>>>> MeshComponentUnits; // 생성된 메시 컴포넌트 유닛들

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