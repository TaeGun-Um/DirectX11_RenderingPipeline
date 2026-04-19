#pragma once
#include <DirectX11_Extension/Ext_Actor.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

// MeshComp를 소유하고 Update마다 지정 축으로 회전하는 액터 베이스 클래스
// 데모용으로 CubeActor/SphereActor 등이 상속해 사용
class RotatingMeshActor : public Ext_Actor
{
public:
	RotatingMeshActor() = default;
	~RotatingMeshActor() = default;

	RotatingMeshActor(const RotatingMeshActor&) = delete;
	RotatingMeshActor(RotatingMeshActor&&) noexcept = delete;
	RotatingMeshActor& operator=(const RotatingMeshActor&) = delete;
	RotatingMeshActor& operator=(RotatingMeshActor&&) noexcept = delete;

	void SetRotate(bool _On = true) { bIsRotation = _On; }
	void SetRotationSpeed(float _DegreesPerSec) { RotationSpeed = _DegreesPerSec; }
	void SetRotationAxis(const float4& _Axis) { RotationAxis = _Axis; }

protected:
	void Update(float _DeltaTime) override;

	std::shared_ptr<class Ext_MeshComponent> MeshComp;
	bool   bIsRotation   = false;
	float  RotationSpeed = 50.0f;                      // 초당 회전 각도(도)
	float4 RotationAxis  = { 0.0f, 1.0f, 0.0f, 0.0f }; // 기본 Y축
};
